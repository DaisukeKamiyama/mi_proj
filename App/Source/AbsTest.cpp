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

AbsTest

★注意：このソースコードはUTF-8 BOM無しで保存されています。ソース内にUTF-8文字列を使用しているため。
Windows Visual Studio ではUTF-8 BOM無しのソースをコンパイルすると正しくコンパイル出来ないことがあるので、
このソースコードをWindowsでコンパイルするときは何らかの対応が必要です。
（gcc 4.0ではUTF-8 BOM有りをコンパイルできないため、UTF-8 BOM無しにせざるをえませんでした。）

*/

#include "stdafx.h"

#include "AApp.h"
#include "AView_Text.h"
#include "ADocPrefDB.h"

#if 0
/*
test
*/

void	test()//4
{//5
	abc();//6
}//7

void	xyz()//9
{//10
	abc();//11
	if( aaaaaaaaaaaaaaaaaaaaa//12,13
	)//14
	{//15
	bbbbbbbbbbbbbbbbbbbbbb();//16,17
	}//18
}//19

#endif

ABool	TextInfoTest()
{
	AText	text;
	text.SetCstring("/*\rtest\r*/\r\rvoid	test()//4\r{//5\r	abc();//6\r}//7\r\rvoid	xyz()//9\r{//10\r	abc();//11\r	if( aaaaaaaaaaaaaaaaaaaaa//12,13\r		)//14\r	{//15\r		bbbbbbbbbbbbbbbbbbbbbb();//16,17\r	}//18\r}//19\r");
	ATextInfoForDocument	textInfo(NULL);
	AText	fontname("Monaco");
	/*AItemCount	completedTextLen = */textInfo.CalcLineInfoAll(text,true,kIndex_Invalid,fontname,
				9.0,true,4,4,kWrapMode_WordWrapByLetterCount,30,800,true);
	if( textInfo.GetLineCount() != 21 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(0) != 0 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(1) != 1 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(2) != 2 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(3) != 3 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(4) != 4 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(5) != 5 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(6) != 6 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(7) != 7 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(8) != 8 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(9) != 9 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(10) != 10 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(11) != 11 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(12) != 12 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(13) != 12 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(14) != 13 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(15) != 14 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(16) != 15 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(17) != 15 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(18) != 16 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(19) != 17 )   _ACThrow("",NULL);
	if( textInfo.GetParagraphIndexByLineIndex(20) != 18 )   _ACThrow("",NULL);
	//
	ABool	importChanged = false;
	AArray<AUniqID>	addedIdentifier;
	AArray<AIndex>		addedIdentifierLineIndex;
	AText	url;//#998
	ABool	abortFlag = false;
	textInfo.RecognizeSyntaxAll(text,url,addedIdentifier,addedIdentifierLineIndex,importChanged,abortFlag);//#998
	//FindFirstUnrecognizedLine()のテスト
	if( textInfo.FindFirstUnrecognizedLine() != kIndex_Invalid )   _ACThrow("",NULL);
	//textInfo.SetLineRecognized(17,false);//17行目
	textInfo.ClearRecognizeFlags(17);
	if( textInfo.FindFirstUnrecognizedLine() != 16 )   _ACThrow("",NULL);//17行目の段落開始=16行め
	//textInfo.SetLineRecognized(12,false);//12行目
	textInfo.ClearRecognizeFlags(12);
	if( textInfo.FindFirstUnrecognizedLine() != 12 )   _ACThrow("",NULL);
	//textInfo.SetLineRecognized(10,false);
	textInfo.ClearRecognizeFlags(10);
	if( textInfo.FindFirstUnrecognizedLine() != 10 )   _ACThrow("",NULL);
	while( textInfo.FindFirstUnrecognizedLine() != kIndex_Invalid )//未認識が無くなるまで繰り返し
	{
		textInfo.PrepareRecognizeSyntaxContext(text,textInfo.FindFirstUnrecognizedLine(),url);//#998
		AArray<AUniqID>	deletedIdentifier;
		ABool	abortFlag = false;
		textInfo.ExecuteRecognizeSyntax(text,kIndex_Invalid,
					deletedIdentifier,addedIdentifier,addedIdentifierLineIndex,importChanged,abortFlag);
	}
	//GetLineIndexByParagraphIndex()のテスト
	if( textInfo.GetLineIndexByParagraphIndex(0) != 0 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(1) != 1 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(2) != 2 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(3) != 3 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(4) != 4 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(5) != 5 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(6) != 6 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(7) != 7 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(8) != 8 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(9) != 9 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(10) != 10 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(11) != 11 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(12) != 12 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(13) != 14 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(14) != 15 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(15) != 16 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(16) != 18 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(17) != 19 )   _ACThrow("",NULL);
	if( textInfo.GetLineIndexByParagraphIndex(18) != 20 )   _ACThrow("",NULL);
	
	return true;
}

//#598
//スレッドテスト

class ATestThread : public AThread
{
	//コンストラクタ／デストラクタ
  public:
	ATestThread( AObjectArrayItem* inParent );
	~ATestThread();
	
  private:
	void					NVIDO_ThreadMain();
};

class ATestThreadFactory : public AThreadFactory
{
  public:
	ATestThreadFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	ATestThread*	Create() 
	{
		return new ATestThread(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};

ATestThread::ATestThread( AObjectArrayItem* inParent ) : AThread(inParent)
{
}
ATestThread::~ATestThread()
{
}

AIndex	sThreadTestMode = 0;
ABool	sThreadTestMainReady = false;
ABool	sThreadTestSubReady = false;

//1
ANumber	sThreadTestNumber = 0;
AThreadMutex	sThreadTestNumberMutex;

//2,3
AObjectArrayIDIndexed<AText>	sThreadTestObjectArray1(NULL);//#693,true);
AObjectArray<AText>	sThreadTestObjectArray2(NULL);//#693,false);
AObjectID	sThreadTestObjectArrayObjectID;

//4
AText	sThreadTestText;

//5
AHashTextArray	sThreadTestHashObjectArray;

//6
ATextArray	sThreadTestTextArray;

//7
AHashArray<ANumber>	sThreadTestHashArray;


void	ATestThread::NVIDO_ThreadMain()
{
	try
	{
		switch(sThreadTestMode)
		{
		  case 1:
			{
				AText	outputText;
				//メインとの同期合わせ
				while( sThreadTestMainReady == false )
				{
					//何もしない（trueになるまで待ち続ける）
				}
				sThreadTestSubReady = true;
				//
				for( AIndex i = 0; i < 100000; i++ )
				{
					ANumber	num = 0;
					{
						AStMutexLocker	locker(sThreadTestNumberMutex);
						sThreadTestNumber++;
						num = sThreadTestNumber;
					}
					AText	text;
					text.SetNumber(num);
					outputText.AddText(text);
					outputText.Add(kUChar_LineEnd);
				}
				//結果書き出し
				AFileAcc	folder;
				GetApp().SPI_GetTempFolder(folder);
				AFileAcc	file;
				file.SpecifyChild(folder,"threadtestresult1");
				file.CreateFile();
				file.WriteFile(outputText);
				break;
			}
		  case 2:
			{
				AText	outputText;
				//メインとの同期合わせ
				while( sThreadTestMainReady == false )
				{
					//何もしない（trueになるまで待ち続ける）
				}
				sThreadTestSubReady = true;
				//
				for( AIndex i = 0; i < 100000; i++ )
				{
					AText	text;
					text.SetText(sThreadTestObjectArray1.GetObjectByID(sThreadTestObjectArrayObjectID));
					ANumber	num = text.GetNumber();
					num++;
					sThreadTestObjectArray1.GetObjectByID(sThreadTestObjectArrayObjectID).SetNumber(num);
					//
					text.SetNumber(sThreadTestObjectArray1.GetItemCount());
					outputText.AddText(text);
					outputText.Add(kUChar_LineEnd);
				}
				//結果書き出し
				AFileAcc	folder;
				GetApp().SPI_GetTempFolder(folder);
				AFileAcc	file;
				file.SpecifyChild(folder,"threadtestresult2");
				file.CreateFile();
				file.WriteFile(outputText);
				break;
			}
#if 0
		  case 3:
			{
				AText	outputText;
				//メインとの同期合わせ
				while( sThreadTestMainReady == false )
				{
					//何もしない（trueになるまで待ち続ける）
				}
				sThreadTestSubReady = true;
				//
				for( AIndex i = 0; i < 100000; i++ )
				{
					AText	text;
					text.SetText(sThreadTestObjectArray2.GetObjectByID(sThreadTestObjectArrayObjectID));
					ANumber	num = text.GetNumber();
					num++;
					sThreadTestObjectArray2.GetObjectByID(sThreadTestObjectArrayObjectID).SetNumber(num);
					//
					text.SetNumber(sThreadTestObjectArray2.GetItemCount());
					outputText.AddText(text);
					outputText.Add(kUChar_LineEnd);
				}
				//結果書き出し
				AFileAcc	folder;
				GetApp().SPI_GetTempFolder(folder);
				AFileAcc	file;
				file.SpecifyChild(folder,"threadtestresult3");
				file.CreateFile();
				file.WriteFile(outputText);
				break;
			}
#endif
		  case 4:
			{
				//メインとの同期合わせ
				while( sThreadTestMainReady == false )
				{
					//何もしない（trueになるまで待ち続ける）
				}
				sThreadTestSubReady = true;
				//
				for( AIndex i = 0; i < 100000; i++ )
				{
					AText	text;
					text.SetText(sThreadTestText);
				}
				break;
			}
		  case 5:
			{
				AText	outputText;
				//メインとの同期合わせ
				while( sThreadTestMainReady == false )
				{
					//何もしない（trueになるまで待ち続ける）
				}
				sThreadTestSubReady = true;
				//
				for( AIndex i = 0; i < 100000; i++ )
				{
					AText	text("c");
					AIndex	index = sThreadTestHashObjectArray.Find(text);
					AText	t;
					t.SetNumber(index);
					outputText.AddText(t);
					outputText.Add(kUChar_LineEnd);
				}
				//結果書き出し
				AFileAcc	folder;
				GetApp().SPI_GetTempFolder(folder);
				AFileAcc	file;
				file.SpecifyChild(folder,"threadtestresult5");
				file.CreateFile();
				file.WriteFile(outputText);
				break;
			}
		  case 6:
			{
				AText	outputText;
				//メインとの同期合わせ
				while( sThreadTestMainReady == false )
				{
					//何もしない（trueになるまで待ち続ける）
				}
				sThreadTestSubReady = true;
				//
				for( AIndex i = 0; i < 100000; i++ )
				{
					AText	text("c");
					AIndex	index = sThreadTestTextArray.Find(text);
					AText	t;
					t.SetNumber(index);
					outputText.AddText(t);
					outputText.Add(kUChar_LineEnd);
				}
				//結果書き出し
				AFileAcc	folder;
				GetApp().SPI_GetTempFolder(folder);
				AFileAcc	file;
				file.SpecifyChild(folder,"threadtestresult6");
				file.CreateFile();
				file.WriteFile(outputText);
				break;
			}
		  case 7:
			{
				AText	outputText;
				//メインとの同期合わせ
				while( sThreadTestMainReady == false )
				{
					//何もしない（trueになるまで待ち続ける）
				}
				sThreadTestSubReady = true;
				//
				for( AIndex i = 0; i < 100000; i++ )
				{
					AIndex	index = sThreadTestHashArray.Find(i);
					AText	t;
					t.SetNumber(index);
					outputText.AddText(t);
					outputText.Add(kUChar_LineEnd);
				}
				//結果書き出し
				AFileAcc	folder;
				GetApp().SPI_GetTempFolder(folder);
				AFileAcc	file;
				file.SpecifyChild(folder,"threadtestresult7");
				file.CreateFile();
				file.WriteFile(outputText);
				break;
			}
			
		}
		/*
		for( AIndex i = 0; i < 1000000; i++ )
		{
			AText	text;
			sThreadTestTextArray.Get(0,text);
			outputText.AddText(text);
			outputText.Add(kUChar_LineEnd);
			sThreadTestTextArray.Get(1,text);
			outputText.AddText(text);
			outputText.Add(kUChar_LineEnd);
			sThreadTestTextArray.Get(2,text);
			outputText.AddText(text);
			outputText.Add(kUChar_LineEnd);
		}
		*/
	}
	catch(...)
	{
		_ACError("ATestThread::NVIDO_ThreadMain() caught exception.",NULL);
	}
}

ABool	ThreadTest()
{
	//Mutexのテスト
	{
		sThreadTestMode = 1;
		sThreadTestNumber = 0;
		
		//スレッド起動
		ATestThreadFactory	factory(&(GetApp()));
		AObjectID	id = GetApp().NVI_CreateThread(factory);
		dynamic_cast<ATestThread&>(GetApp().NVI_GetThreadByID(id)).NVI_Run();
		//スレッドとの同期合わせ
		sThreadTestMainReady = true;
		while( sThreadTestSubReady == false )
		{
			//何もしない（trueになるまで待ち続ける）
		}
		//
		for( AIndex i = 0; i < 100000; i++ )
		{
			AStMutexLocker	locker(sThreadTestNumberMutex);
			sThreadTestNumber++;
		}
		//スレッド終了待ち
		GetApp().NVI_GetThreadByID(id).NVI_WaitThreadEnd();
		//値チェック
		//Mutexがちゃんとかかっていないと、少ない値（199855など）になる。
		if( sThreadTestNumber != 200000 )   _ACThrow("",NULL);
	}
	//ObjectArrayのMutex制御テスト（IDtoPtr有り時）
	{
		sThreadTestMode = 2;
		sThreadTestObjectArray1.DeleteAll();
		//
		//Mutexはデフォルトでenableとなっている。
		//下記をEnableThreadSafe(false)にすると、ポインタエラー発生する。
		//典型的な原因はhasisizeが0になること。
		sThreadTestObjectArray1.EnableThreadSafe(true);
		//
		sThreadTestObjectArray1.AddNewObject();
		sThreadTestObjectArray1.AddNewObject();
		sThreadTestObjectArray1.AddNewObject();
		sThreadTestObjectArrayObjectID = sThreadTestObjectArray1.GetObject(1).GetObjectID();
		sThreadTestObjectArray1.GetObject(1).SetCstring("100");
		
		//スレッド起動
		ATestThreadFactory	factory(&(GetApp()));
		AObjectID	id = GetApp().NVI_CreateThread(factory);
		dynamic_cast<ATestThread&>(GetApp().NVI_GetThreadByID(id)).NVI_Run();
		//スレッドとの同期合わせ
		sThreadTestMainReady = true;
		while( sThreadTestSubReady == false )
		{
			//何もしない（trueになるまで待ち続ける）
		}
		//
		for( AIndex i = 0; i < 100000; i++ )
		{
			sThreadTestObjectArray1.AddNewObject();
		}
		//スレッド終了待ち
		GetApp().NVI_GetThreadByID(id).NVI_WaitThreadEnd();
		//値チェック
		if( sThreadTestObjectArray1.GetObjectByID(sThreadTestObjectArrayObjectID).GetNumber() != 100100 )   _ACThrow("",NULL);
	}
#if 0
	//ObjectArrayのMutex制御テスト（IDtoPtr無し時）
	{
		sThreadTestMode = 3;
		sThreadTestObjectArray2.DeleteAll();
		//
		//Mutexはデフォルトでenableとなっている。
		//下記をEnableThreadSafe(false)にすると、ポインタエラー発生する。
		//典型的な原因はメインスレッドでのmObjectPtrArray内のmDataPtrのreallocにより、
		//サブスレッドでmDataPtrが有効なポインタでは無くなること。
		sThreadTestObjectArray2.EnableThreadSafe(true);
		//
		sThreadTestObjectArray2.AddNewObject();
		sThreadTestObjectArray2.AddNewObject();
		sThreadTestObjectArray2.AddNewObject();
		sThreadTestObjectArrayObjectID = sThreadTestObjectArray2.GetObject(1).GetObjectID();
		sThreadTestObjectArray2.GetObject(1).SetCstring("100");
		
		//スレッド起動
		ATestThreadFactory	factory(&(GetApp()));
		AObjectID	id = GetApp().NVI_CreateThread(factory);
		dynamic_cast<ATestThread&>(GetApp().NVI_GetThreadByID(id)).NVI_Run();
		//スレッドとの同期合わせ
		sThreadTestMainReady = true;
		while( sThreadTestSubReady == false )
		{
			//何もしない（trueになるまで待ち続ける）
		}
		//
		for( AIndex i = 0; i < 100000; i++ )
		{
			sThreadTestObjectArray2.AddNewObject();
		}
		//スレッド終了待ち
		GetApp().NVI_GetThreadByID(id).NVI_WaitThreadEnd();
		//値チェック
		if( sThreadTestObjectArray2.GetObjectByID(sThreadTestObjectArrayObjectID).GetNumber() != 100100 )   _ACThrow("",NULL);
	}
#endif
	//
	{
		sThreadTestMode = 4;
		sThreadTestText.DeleteAll();
		sThreadTestText.EnableThreadSafe(true);
		
		//スレッド起動
		ATestThreadFactory	factory(&(GetApp()));
		AObjectID	id = GetApp().NVI_CreateThread(factory);
		dynamic_cast<ATestThread&>(GetApp().NVI_GetThreadByID(id)).NVI_Run();
		//スレッドとの同期合わせ
		sThreadTestMainReady = true;
		while( sThreadTestSubReady == false )
		{
			//何もしない（trueになるまで待ち続ける）
		}
		//
		for( AIndex i = 0; i < 100000; i++ )
		{
			sThreadTestText.Add('\r');
		}
		//スレッド終了待ち
		GetApp().NVI_GetThreadByID(id).NVI_WaitThreadEnd();
		//値チェック
	}
#if 0
	//HashObjectArrayのMutex制御テスト
	{
		sThreadTestMode = 5;
		sThreadTestHashObjectArray.DeleteAll();
		//
		sThreadTestHashObjectArray.EnableThreadSafe(true);
		//
		AText	a("a"),b("b"),c("c");
		sThreadTestHashObjectArray.Add(a);
		sThreadTestHashObjectArray.Add(b);
		sThreadTestHashObjectArray.Add(c);
		
		//スレッド起動
		ATestThreadFactory	factory(&(GetApp()));
		AObjectID	id = GetApp().NVI_CreateThread(factory);
		dynamic_cast<ATestThread&>(GetApp().NVI_GetThreadByID(id)).NVI_Run();
		//スレッドとの同期合わせ
		sThreadTestMainReady = true;
		while( sThreadTestSubReady == false )
		{
			//何もしない（trueになるまで待ち続ける）
		}
		//
		for( AIndex i = 0; i < 100000; i++ )
		{
			AText	t;
			t.SetNumber(i);
			sThreadTestHashObjectArray.Add(t);
		}
		//スレッド終了待ち
		GetApp().NVI_GetThreadByID(id).NVI_WaitThreadEnd();
		//値チェック
	}
	//ATextArrayのMutex制御テスト
	{
		sThreadTestMode = 6;
		sThreadTestTextArray.DeleteAll();
		//
		sThreadTestTextArray.EnableThreadSafe(true);
		//
		AText	a("a"),b("b"),c("c");
		sThreadTestTextArray.Add(a);
		sThreadTestTextArray.Add(b);
		sThreadTestTextArray.Add(c);
		
		//スレッド起動
		ATestThreadFactory	factory(&(GetApp()));
		AObjectID	id = GetApp().NVI_CreateThread(factory);
		dynamic_cast<ATestThread&>(GetApp().NVI_GetThreadByID(id)).NVI_Run();
		//スレッドとの同期合わせ
		sThreadTestMainReady = true;
		while( sThreadTestSubReady == false )
		{
			//何もしない（trueになるまで待ち続ける）
		}
		//
		for( AIndex i = 0; i < 100000; i++ )
		{
			AText	t;
			t.SetNumber(i);
			sThreadTestTextArray.Add(t);
		}
		//スレッド終了待ち
		GetApp().NVI_GetThreadByID(id).NVI_WaitThreadEnd();
		//値チェック
	}
#endif
	//
	{
		sThreadTestMode = 7;
		sThreadTestHashArray.DeleteAll();
		sThreadTestHashArray.EnableThreadSafe(true);
		
		//スレッド起動
		ATestThreadFactory	factory(&(GetApp()));
		AObjectID	id = GetApp().NVI_CreateThread(factory);
		dynamic_cast<ATestThread&>(GetApp().NVI_GetThreadByID(id)).NVI_Run();
		//スレッドとの同期合わせ
		sThreadTestMainReady = true;
		while( sThreadTestSubReady == false )
		{
			//何もしない（trueになるまで待ち続ける）
		}
		//
		for( AIndex i = 0; i < 100000; i++ )
		{
			sThreadTestHashArray.Add(i);
		}
		//スレッド終了待ち
		GetApp().NVI_GetThreadByID(id).NVI_WaitThreadEnd();
	}
	/*
	AText	text1, text2, text3;
	sThreadTestTextArray.Add(text1);
	sThreadTestTextArray.Add(text2);
	sThreadTestTextArray.Add(text3);
	
	for( AIndex i = 0; i < 1000000; i++ )
	{
		AText	text;
		text.SetNumber(i);
		sThreadTestTextArray.Set(0,text);
		sThreadTestTextArray.Set(1,text);
		sThreadTestTextArray.Set(2,text);
	}
	*/
	return true;
}

const  AControlID	kMenu_ModeSelect					= 101;
const AControlID	kButton_CurrentWindowSize			= 113;
const  AControlID	kButton_BackgroundImageFile			= 132;
const  AControlID	kText_KeywordList					= 171;
const  AControlID	kTable_KeywordCategory				= 172;
const  AControlID	kColor_KeywordColor					= 173;
const  AControlID	kCheck_KeywordBold					= 174;
const  AControlID	kCheck_KeywordIgnoreCase			= 175;
const  AControlID	kCheck_KeywordRegExp				= 176;
const  AControlID	kButton_KeywordSave					= 177;
const  AControlID	kButton_KeywordSort					= 178;
const  AControlID	kButton_KeywordAddCategory			= 179;
const  AControlID	kButton_KeywordRemoveCategory		= 180;
const  AControlID	kGroup_KeywordCategorySetup			= 200;
const  AControlID	kButton_KeywordRemoveCategoryOK		= 201;

const  AControlID	kTable_JumpSetup					= 202;
const  AControlID	kText_JumpSetup_RegExp				= 203;
const  AControlID	kCheck_JumpSetup_RegisterToMenu		= 204;
const  AControlID	kText_JumpSetup_MenuText			= 205;
const  AControlID	kText_JumpSetup_SelectionGroup		= 206;
const  AControlID	kCheck_JumpSetup_ColorizeLine		= 207;
const  AControlID	kColor_JumpSetup_LineColor			= 208;
const  AControlID	kCheck_JumpSetup_RegisterAsKeyword	= 209;
const  AControlID	kColor_JumpSetup_KeywordColor		= 210;
const  AControlID	kText_JumpSetup_KeywordGroup		= 211;
const  AControlID	kButton_JumpSetup_Add				= 212;
const  AControlID	kButton_JumpSetup_Remove			= 213;
const  AControlID	kGroup_JumpSetup					= 225;
const  AControlID	kButton_JumpSetup_RemoveOK			= 226;

const  AControlID	kTable_Suffix						= 214;
const  AControlID	kButton_SuffixAdd					= 215;
const  AControlID	kButton_SuffixRemove				= 216;
const  AControlID	kButton_SuffixRemoveOK				= 275;

const  AControlID	kTable_DragDrop					= 227;
const  AControlID	kButton_DragDropAdd				= 228;
const  AControlID	kButton_DragDrapRemove			= 229;
const  AControlID	kText_DragDrapText				= 230;
const  AControlID	kButton_DragDrapRemoveOK		= 231;
const  AControlID	kGroup_DragDrop					= 234;

const  AControlID	kTable_Correspond				= 219;
const  AControlID	kButton_CorrespondAdd			= 220;
const  AControlID	kButton_CorrespondRemove		= 221;
const  AControlID	kButton_CorrespondRemoveOK		= 232;

const  AControlID	kTable_Indent					= 222;
const  AControlID	kButton_IndentAdd				= 223;
const  AControlID	kButton_IndentRemove			= 224;
const  AControlID	kButton_IndentRemoveOK			= 233;

const  AControlID	kCheck_Keybind_ControlKey		= 235;
const  AControlID	kCheck_Keybind_OptionKey		= 236;
const  AControlID	kCheck_Keybind_CommandKey		= 237;
const  AControlID	kCheck_Keybind_ShiftKey			= 238;
const  AControlID	kTable_Keybind					= 239;
const  AControlID	kPopup_KeybindAction			= 240;
const  AControlID	kText_KeybindActionText			= 241;
const  AControlID	kGroupKeybindItem				= 242;
const  AControlID	kButton_Keybind_Import			= 262;
const  AControlID	kButton_Keybind_Export			= 263;

const  AControlID	kToolbarMode					= 265;
const  AControlID	kButton_ToolAdd					= 267;
const  AControlID	kButton_ToolRemove				= 268;
const  AControlID	kButton_Tool_Display			= 269;
const  AControlID	kText_Tool						= 270;
const  AControlID	kText_Tool_Shortcut				= 271;
const  AControlID	kCheck_Tool_ShortcutControlKey	= 272;
const  AControlID	kCheck_Tool_ShortcutShiftKey	= 273;
const  AControlID	kGroup_Tool						= 274;
													//275 reserved

const  AControlID	kButton_UpdateSyntaxDefinition	= 276;

const  AControlID	kListViewFrame_StateColor		= 277;
const  AControlID	kListView_StateColor			= 278;
const  AControlID	kVScrollBar_StateColor			= 279;

const  AControlID	kListViewFrame_CategoryColor	= 280;
const  AControlID	kListView_CategoryColor			= 281;
const  AControlID	kVScrollBar_CategoryColor		= 282;

const  AControlID	kStateColor						= 283;
const  AControlID	kCategoryColor					= 284;
const  AControlID	kCategoryImportColor			= 288;
const  AControlID	kCategoryLocalColor				= 289;

const  AControlID	kListViewFrame_Tool				= 285;
const  AControlID	kListView_Tool					= 286;
const  AControlID	kVScrollBar_Tool				= 287;

const  AControlID	kStateContentGroup				= 290;
const  AControlID	kCategoryContentGroup			= 291;

const  AControlID	kButton_UpdateJumpSetup			= 292;

const  AControlID	kButton_ToolParentFolder		= 293;
const  AControlID	kButton_ToolPath				= 294;
const  AControlID	kText_ToolName					= 295;
const  AControlID	kButton_ToolSave				= 296;
const  AControlID	kToolMode						= 297;
const  AControlID	kButton_ToolRemoveOK			= 298;
const  AControlID	kButton_ChangeToolButtonIcon	= 299;
const  AControlID	kButton_AddToolFolder			= 300;

const  AControlID	kButton_AddNewMode				= 500;
const  AControlID	kButton_RevealModeFolder		= 501;
const  AControlID	kButton_RevealToolFolder		= 502;
const  AControlID	kButton_RevealToolbarFolder		= 503;
const  AControlID	kButton_ReconfigWithToolFolder	= 504;
const  AControlID	kButton_EditSuffixTable			= 505;
const  AControlID	kButton_KeyBindTextOK			= 506;
const  AControlID	kButton_KeywordCategoryEdit		= 507;
const  AControlID	kButton_JumpSetupNameEdit		= 508;
const  AControlID	kButton_DragDropSuffixEdit		= 509;
const  AControlID	kButton_CorrStartEdit			= 510;
const  AControlID	kButton_CorrEndEdit				= 511;
const  AControlID	kButton_IndentRegExpEdit		= 512;
const  AControlID	kButton_IndentCurLineEdit		= 513;
const  AControlID	kButton_IndentNextLineEdit		= 514;
const  AControlID	kButton_NewFile					= 515;
const  AControlID	kCheck_KeywordUnderline				= 516;//R0195
const  AControlID	kCheck_KeywordItalic				= 517;//R0195
const  AControlID	kCheck_KeywordPriorToOtherColor		= 518;//R0195
const  AControlID	kCheck_SyntaxCategoryBold			= 519;//R0195
const  AControlID	kCheck_SyntaxCategoryItalic			= 520;//R0195
const  AControlID	kCheck_SyntaxCategoryUnderline		= 521;//R0195
const  AControlID	kText_JumpSetup_Anchor				= 522;//R0212
const  AControlID	kButton_DragDropImport				= 523;//R0000
const  AControlID	kButton_DragDropExport				= 524;//R0000
const  AControlID	kButton_CorrespondImport			= 525;//R0000
const  AControlID	kButton_CorrespondExport			= 526;//R0000
const  AControlID	kButton_RegExpIndentImport			= 527;//R0000
const  AControlID	kButton_RegExpIndentExport			= 528;//R0000
const  AControlID	kTable_ModeSelectFilePath			= 529;//R0210
const  AControlID	kButton_ModeSelectAdd				= 530;//R0210
const  AControlID	kButton_ModeSelectRemove			= 531;//R0210
const  AControlID	kButton_ModeSelectRemoveOK			= 532;//R0210
const  AControlID	kButton_EditModeSelectFilePath		= 533;//R0210
const  AControlID	kButton_KeywordCategory_Up			= 534;//R0183
const  AControlID	kButton_KeywordCategory_Down		= 535;//R0183
const  AControlID	kButton_KeywordCategory_Top			= 536;//R0183
const  AControlID	kButton_KeywordCategory_Bottom		= 537;//R0183
const  AControlID	kButton_JumpSetup_Up				= 538;//R0183
const  AControlID	kButton_JumpSetup_Down				= 539;//R0183
const  AControlID	kButton_JumpSetup_Top				= 540;//R0183
const  AControlID	kButton_JumpSetup_Bottom			= 541;//R0183
const  AControlID	kButton_Keybind_ImportAdd			= 542;//R0000
const  AControlID	kButton_Suffix_Up					= 543;//R0000
const  AControlID	kButton_Suffix_Down					= 544;//R0000
const  AControlID	kButton_Suffix_Top					= 545;//R0000
const  AControlID	kButton_Suffix_Bottom				= 546;//R0000
const  AControlID	kButton_KeywordDisable				= 547;//R0000
const  AControlID	kButton_SyntaxDefColorsImport		= 548;//R0000
const  AControlID	kButton_SyntaxDefColorsExport		= 549;//R0000
const  AControlID	kButton_KeywordsImport				= 550;//R0068
const  AControlID	kButton_KeywordsExport				= 551;//R0068
//extern const char* kAllSJISChars;

extern void	MakeAllUnicodeCharText( AText& outText );
extern void	MakeAllSJISCharText( AText& outSJISText );

ABool	ModePrefWindowTest()
{
	ABool	result = true;
//#205 #if USE_IMP_TABLEVIEW
	AText	text("%%%UnitTest%%%");
	AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(text);
	if( modeIndex == kIndex_Invalid )
	{
		modeIndex = GetApp().SPI_AddNewMode(text);
	}
	text.SetCstring("%%%UnitTest2%%%");
	AModeIndex	modeIndex2 = GetApp().SPI_FindModeIndexByModeRawName(text);
	if( modeIndex2 == kIndex_Invalid )
	{
		modeIndex2 = GetApp().SPI_AddNewMode(text);
	}
	text.SetCstring("%%%UnitTest2%%%");
	GetApp().SPI_ShowModePrefWindow(modeIndex);
	
	AFileAcc	testfolder;
	testfolder.SpecifyAsUnitTestFolder();
	
	//========================================
	//   キーワード
	//========================================
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kAdditionalCategory_Name)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kAdditionalCategory_Name,0);
		//#427 GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalCategoryKeywordArray().Delete1Object(0);
	}
	//全削除 "%%%UnitTest2%%%"のほう
	while(GetApp().SPI_GetModePrefDB(modeIndex2).GetItemCount_Array(AModePrefDB::kAdditionalCategory_Name)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex2).DeleteRow_Table(AModePrefDB::kAdditionalCategory_Name,0);
		//#427 GetApp().SPI_GetModePrefDB(modeIndex2).GetAdditionalCategoryKeywordArray().Delete1Object(0);
	}
	
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SelectTabControl(6);//キーワードタブ
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordAddCategory,0);//新規ボタン
	text.SetCstring("test1\rtest2\rtest3\r");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeywordList,text);//キーワード
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_KeywordList);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordSave,0);//保存ボタン
	//B0406 未保存でのカテゴリー削除→次にカテゴリー選択で保存ダイアログ表示のバグ
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordAddCategory,0);//新規ボタン
	text.SetCstring("abc");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeywordList,text);//キーワード
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_KeywordList);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordRemoveCategory,0);//削除ボタン
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordRemoveCategoryOK,0);//削除OK
	/*#427
	AObjectID	objectID = GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordCategoryObjectIDByIndex(0);
	GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordForModePrefWindow(objectID,text);
	*/
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,0,text);//#427
	if( text.Compare("test1\rtest2\rtest3\r") == false )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);//キーワード
	if( text.Compare("test1\rtest2\rtest3\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordRemoveCategoryOK,0);//削除OK
	
	//カテゴリー追加(a,b,c)
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordAddCategory,0);//新規ボタン
	text.SetCstring("c\rb\ra\r");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeywordList,text);//キーワード
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_KeywordList);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordSave,0);//保存ボタン
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordSort,0);//ソートボタン
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordSave,0);//保存ボタン
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlColor(kColor_KeywordColor,kColor_Red);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kColor_KeywordColor);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordBold,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordBold);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordItalic,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordItalic);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordUnderline,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordUnderline);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordPriorToOtherColor,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordPriorToOtherColor);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordIgnoreCase,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordIgnoreCase);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordRegExp,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordRegExp);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kButton_KeywordDisable,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kButton_KeywordDisable);
	//カテゴリー追加(d,e,f)
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordAddCategory,0);//新規ボタン
	text.SetCstring("d\re\rf\r");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeywordList,text);//キーワード
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_KeywordList);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordSave,0);//保存ボタン
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordSort,0);//ソートボタン
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordSave,0);//保存ボタン
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlColor(kColor_KeywordColor,kColor_Green);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kColor_KeywordColor);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordBold,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordBold);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordItalic,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordItalic);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordUnderline,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordUnderline);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordPriorToOtherColor,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordPriorToOtherColor);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordIgnoreCase,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordIgnoreCase);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordRegExp,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordRegExp);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kButton_KeywordDisable,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kButton_KeywordDisable);
	//カテゴリー追加(g,h,i)
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordAddCategory,0);//新規ボタン
	text.SetCstring("i\rh\rg\r");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeywordList,text);//キーワード
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_KeywordList);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordSort,0);//ソートボタン
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordSave,0);//保存ボタン
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlColor(kColor_KeywordColor,kColor_Blue);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kColor_KeywordColor);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordBold,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordBold);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordItalic,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordItalic);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordUnderline,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordUnderline);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordPriorToOtherColor,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordPriorToOtherColor);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordIgnoreCase,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordIgnoreCase);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_KeywordRegExp,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_KeywordRegExp);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kButton_KeywordDisable,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kButton_KeywordDisable);
	//カテゴリー名編集
	ATextArray	ta;
	ta.Add("([abc])");
	ta.Add("def");
	ta.Add("GHI");
	/*#205 GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().BeginSetTable(kTable_KeywordCategory);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().SetTable_Text(kTable_KeywordCategory,0,ta);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().EndSetTable(kTable_KeywordCategory);*/
	GetApp().SPI_GetModePrefWindow(0).NVI_BeginSetTable(kTable_KeywordCategory);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetTableColumn_Text(kTable_KeywordCategory,0,ta);
	GetApp().SPI_GetModePrefWindow(0).NVI_EndSetTable(kTable_KeywordCategory);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kTable_KeywordCategory);
	
	//【保存済状態での各種動作確認】
	
	//2を↑に移動
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordCategory_Up,0);//↑ボタン
	//データ確認
	/*#427
	objectID = GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordCategoryObjectIDByIndex(0);
	GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordForModePrefWindow(objectID,text);
	*/
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,0,text);//#427
	if( text.Compare("a\rb\rc\r") == false )   _ACThrow("",NULL);
	/*#427
	objectID = GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordCategoryObjectIDByIndex(1);
	GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordForModePrefWindow(objectID,text);
	*/
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,1,text);//#427
	if( text.Compare("g\rh\ri\r") == false )   _ACThrow("",NULL);
	/*#427
	objectID = GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordCategoryObjectIDByIndex(2);
	GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordForModePrefWindow(objectID,text);
	*/
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,2,text);//#427
	if( text.Compare("d\re\rf\r") == false )   _ACThrow("",NULL);
	
	//2をtopに移動
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordCategory_Top,0);//topボタン
	//データ確認
	/*#427
	objectID = GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordCategoryObjectIDByIndex(0);
	GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordForModePrefWindow(objectID,text);
	*/
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,0,text);//#427
	if( text.Compare("d\re\rf\r") == false )   _ACThrow("",NULL);
	/*#427
	objectID = GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordCategoryObjectIDByIndex(1);
	GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordForModePrefWindow(objectID,text);
	*/
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,1,text);//#427
	if( text.Compare("a\rb\rc\r") == false )   _ACThrow("",NULL);
	/*#427
	objectID = GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordCategoryObjectIDByIndex(2);
	GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordForModePrefWindow(objectID,text);
	*/
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,2,text);//#427
	if( text.Compare("g\rh\ri\r") == false )   _ACThrow("",NULL);
	
	//1を↓に移動
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordCategory_Down,0);//↓ボタン
	//データ確認
	/*#427
	objectID = GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordCategoryObjectIDByIndex(0);
	GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordForModePrefWindow(objectID,text);
	*/
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,0,text);//#427
	if( text.Compare("d\re\rf\r") == false )   _ACThrow("",NULL);
	/*#427
	objectID = GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordCategoryObjectIDByIndex(1);
	GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordForModePrefWindow(objectID,text);
	*/
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,1,text);//#427
	if( text.Compare("g\rh\ri\r") == false )   _ACThrow("",NULL);
	/*#427
	objectID = GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordCategoryObjectIDByIndex(2);
	GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalKeywordForModePrefWindow(objectID,text);
	*/
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Keywords,2,text);//#427
	if( text.Compare("a\rb\rc\r") == false )   _ACThrow("",NULL);
	
	AColor	color;
	//1をbottomに移動
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordCategory_Bottom,0);//bottomボタン
	//選択中に閉じて開いてみる
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_DoCloseButton();
	GetApp().SPI_ShowModePrefWindow(modeIndex);
	//データ確認（ContentViewに展開して確認）
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("d\re\rf\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("a\rb\rc\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != false )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("g\rh\ri\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	
	AColor	r, g, b;
	text.SetCstring("FF0000");
	r = AColorWrapper::GetColorByHTMLFormatColor(text);
	text.SetCstring("00FF00");
	g = AColorWrapper::GetColorByHTMLFormatColor(text);
	text.SetCstring("0000FF");
	b = AColorWrapper::GetColorByHTMLFormatColor(text);
	//エクスポート・インポート
	AFileAcc	file;
	file.SpecifyChild(testfolder,"KeywordExportTest.txt");
	file.CreateFile();
	text.SetCstring("keywordsexport");
	GetApp().SPI_GetModePrefWindow(0).EVT_SaveWindowReply(kObjectID_Invalid,file,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_FileChoosen(kButton_KeywordsImport,file);
	//データ確認（ContentViewに展開して確認）
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("d\re\rf\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,g) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("a\rb\rc\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,r) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != false )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("g\rh\ri\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,b) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	
	//全削除してインポート
	//2削除
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordRemoveCategory,0);//削除ボタン
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordRemoveCategoryOK,0);//削除OK
	//1削除
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordRemoveCategory,0);//削除ボタン
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordRemoveCategoryOK,0);//削除OK
	//0削除
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordRemoveCategory,0);//削除ボタン
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordRemoveCategoryOK,0);//削除OK
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,kIndex_Invalid);//テーブル選択解除
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,kIndex_Invalid);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_FileChoosen(kButton_KeywordsImport,file);
	//データ確認（ContentViewに展開して確認）
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("d\re\rf\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,g) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("a\rb\rc\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,r) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != false )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("g\rh\ri\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,b) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	//カテゴリー名確認
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Name,0,text);
	if( text.Compare("def") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Name,1,text);
	if( text.Compare("((abc))") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Name,2,text);
	if( text.Compare("GHI") == false )   _ACThrow("",NULL);
	
	//B0303-3類似パターン ContentView表示中にモード切替
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	text.SetText("%%%UnitTest2%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0)./*#205 NVI_GetControlNumber*/NVI_GetTableViewSelectionDBIndex(kTable_KeywordCategory) != kIndex_Invalid )   _ACThrow("",NULL);
	text.SetText("%%%UnitTest%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0)./*#205 NVI_GetControlNumber*/NVI_GetTableViewSelectionDBIndex(kTable_KeywordCategory) != kIndex_Invalid )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	//データ確認（ContentViewに展開して確認）
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("d\re\rf\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,g) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("a\rb\rc\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,r) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != false )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("g\rh\ri\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,b) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	//カテゴリー名確認
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Name,0,text);
	if( text.Compare("def") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Name,1,text);
	if( text.Compare("((abc))") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Name,2,text);
	if( text.Compare("GHI") == false )   _ACThrow("",NULL);
	
	//【未保存状態での各種動作確認】
	
	//未保存状態にする
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	text.SetCstring("1\r2\r3\r");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeywordList,text);//キーワード
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_KeywordList);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordCategory_Up,0);//↑ボタン
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordCategory_Down,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordCategory_Top,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordCategory_Bottom,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordAddCategory,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,kIndex_Invalid);//
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,kIndex_Invalid);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	//モード追加、閉じる等
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_AddNewMode,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_DoMenuItemSelected(kMenuItemID_Close,GetEmptyText(),0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_DoCloseButton();
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	//モード変更
	text.SetText("%%%UnitTest2%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordSave,0);//
	//データ確認（ContentViewに展開して確認）
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("1\r2\r3\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,g) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("a\rb\rc\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,r) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != false )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("g\rh\ri\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,b) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	//カテゴリー名確認
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Name,0,text);
	if( text.Compare("def") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Name,1,text);
	if( text.Compare("((abc))") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kAdditionalCategory_Name,2,text);
	if( text.Compare("GHI") == false )   _ACThrow("",NULL);
	
	//異常ルートテスト
	//不具合があっても既存のデータは破壊・消去されないことの確認
	//行選択が異常ルートで変更された場合
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_KeywordList);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeywordSave,0);//保存ボタン
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlColor(kColor_KeywordColor,b);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kColor_KeywordColor);
	//データ確認（ContentViewに展開して確認）
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("1\r2\r3\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,g) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("a\rb\rc\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,r) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != false )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("g\rh\ri\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,b) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	//ContentViewが異常ルートで変更された場合
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,kIndex_Invalid);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,kIndex_Invalid);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).NVI_UpdateProperty();
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	//データ確認（ContentViewに展開して確認）
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("1\r2\r3\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,g) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("a\rb\rc\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,r) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != false )   _ACThrow("",NULL);
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeywordList,text);
	if( text.Compare("g\rh\ri\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,b) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordBold) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordItalic) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordUnderline) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordPriorToOtherColor) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordIgnoreCase) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_KeywordRegExp) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_KeywordDisable) != true )   _ACThrow("",NULL);
	
	//========================================
	//   見出し
	//========================================
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kJumpSetup_Name)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kJumpSetup_Name,0);
	}
	//全削除 "%%%UnitTest2%%%"のほう
	while(GetApp().SPI_GetModePrefDB(modeIndex2).GetItemCount_Array(AModePrefDB::kJumpSetup_Name)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex2).DeleteRow_Table(AModePrefDB::kJumpSetup_Name,0);
	}
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SelectTabControl(7);//見出しタブ
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_JumpSetup_Add,0);//新規ボタン
	text.SetCstring("regexp1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_RegExp,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_RegExp);
	text.SetCstring("menutext1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_MenuText,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_MenuText);
	text.SetCstring("1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_SelectionGroup,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_SelectionGroup);
	text.SetCstring("11");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_KeywordGroup,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_KeywordGroup);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_JumpSetup_RegisterToMenu,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_JumpSetup_RegisterToMenu);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_JumpSetup_ColorizeLine,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_JumpSetup_ColorizeLine);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_JumpSetup_RegisterAsKeyword,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_JumpSetup_RegisterAsKeyword);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlColor(kColor_JumpSetup_LineColor,kColor_Red);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kColor_JumpSetup_LineColor);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlColor(kColor_JumpSetup_KeywordColor,kColor_Red);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kColor_JumpSetup_KeywordColor);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_JumpSetup_Add,0);//新規ボタン
	text.SetCstring("regexp2");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_RegExp,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_RegExp);
	text.SetCstring("menutext2");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_MenuText,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_MenuText);
	text.SetCstring("2");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_SelectionGroup,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_SelectionGroup);
	text.SetCstring("22");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_KeywordGroup,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_KeywordGroup);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_JumpSetup_RegisterToMenu,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_JumpSetup_RegisterToMenu);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_JumpSetup_ColorizeLine,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_JumpSetup_ColorizeLine);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_JumpSetup_RegisterAsKeyword,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_JumpSetup_RegisterAsKeyword);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlColor(kColor_JumpSetup_LineColor,kColor_Green);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kColor_JumpSetup_LineColor);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlColor(kColor_JumpSetup_KeywordColor,kColor_Green);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kColor_JumpSetup_KeywordColor);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_JumpSetup_Add,0);//新規ボタン
	text.SetCstring("regexp3");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_RegExp,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_RegExp);
	text.SetCstring("menutext3");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_MenuText,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_MenuText);
	text.SetCstring("3");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_SelectionGroup,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_SelectionGroup);
	text.SetCstring("33");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_KeywordGroup,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_JumpSetup_KeywordGroup);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_JumpSetup_RegisterToMenu,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_JumpSetup_RegisterToMenu);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_JumpSetup_ColorizeLine,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_JumpSetup_ColorizeLine);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_JumpSetup_RegisterAsKeyword,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_JumpSetup_RegisterAsKeyword);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlColor(kColor_JumpSetup_LineColor,kColor_Blue);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kColor_JumpSetup_LineColor);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlColor(kColor_JumpSetup_KeywordColor,kColor_Blue);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kColor_JumpSetup_KeywordColor);
	//見出し名
	ta.DeleteAll();
	ta.Add("jumpsetupname1");
	ta.Add("jumpsetupname2");
	ta.Add("jumpsetupname3");
	/*#205 GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().BeginSetTable(kTable_JumpSetup);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().SetTable_Text(kTable_JumpSetup,0,ta);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().EndSetTable(kTable_JumpSetup);*/
	GetApp().SPI_GetModePrefWindow(0).NVI_BeginSetTable(kTable_JumpSetup);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetTableColumn_Text(kTable_JumpSetup,0,ta);
	GetApp().SPI_GetModePrefWindow(0).NVI_EndSetTable(kTable_JumpSetup);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kTable_JumpSetup);
	//データ確認
	//
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kJumpSetup_Name,0,text);
	if( text.Compare("jumpsetupname1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kJumpSetup_RegExp,0,text);
	if( text.Compare("regexp1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kJumpSetup_MenuText,0,text);
	if( text.Compare("menutext1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kJumpSetup_SelectionGroup,0) != 1 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kJumpSetup_KeywordGroup,0) != 11 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_BoolArray(AModePrefDB::kJumpSetup_ColorizeLine,0) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_BoolArray(AModePrefDB::kJumpSetup_RegisterToMenu,0) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_BoolArray(AModePrefDB::kJumpSetup_RegisterAsKeyword,0) != false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_ColorArray(AModePrefDB::kJumpSetup_LineColor,0,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_ColorArray(AModePrefDB::kJumpSetup_KeywordColor,0,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	//
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kJumpSetup_Name,1,text);
	if( text.Compare("jumpsetupname2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kJumpSetup_RegExp,1,text);
	if( text.Compare("regexp2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kJumpSetup_MenuText,1,text);
	if( text.Compare("menutext2") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kJumpSetup_SelectionGroup,1) != 2 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kJumpSetup_KeywordGroup,1) != 22 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_BoolArray(AModePrefDB::kJumpSetup_ColorizeLine,1) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_BoolArray(AModePrefDB::kJumpSetup_RegisterToMenu,1) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_BoolArray(AModePrefDB::kJumpSetup_RegisterAsKeyword,1) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_ColorArray(AModePrefDB::kJumpSetup_LineColor,1,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_ColorArray(AModePrefDB::kJumpSetup_KeywordColor,1,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	//
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kJumpSetup_Name,2,text);
	if( text.Compare("jumpsetupname3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kJumpSetup_RegExp,2,text);
	if( text.Compare("regexp3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kJumpSetup_MenuText,2,text);
	if( text.Compare("menutext3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kJumpSetup_SelectionGroup,2) != 3 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kJumpSetup_KeywordGroup,2) != 33 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_BoolArray(AModePrefDB::kJumpSetup_ColorizeLine,2) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_BoolArray(AModePrefDB::kJumpSetup_RegisterToMenu,2) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_BoolArray(AModePrefDB::kJumpSetup_RegisterAsKeyword,2) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_ColorArray(AModePrefDB::kJumpSetup_LineColor,2,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_ColorArray(AModePrefDB::kJumpSetup_KeywordColor,2,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	//データ確認（ContentViewに展開して確認）
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("11") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,1);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("22") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,2);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("33") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	//B0303-3 ContentView表示中にモード切替
	text.SetText("%%%UnitTest2%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0)./*#205 NVI_GetControlNumber*/NVI_GetTableViewSelectionDBIndex(kTable_KeywordCategory) != kIndex_Invalid )   _ACThrow("",NULL);
	text.SetText("%%%UnitTest%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0)./*#205 NVI_GetControlNumber*/NVI_GetTableViewSelectionDBIndex(kTable_KeywordCategory) != kIndex_Invalid )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("33") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("11") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("22") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	
	//異常ルートテスト
	//不具合があっても既存のデータは破壊・消去されないことの確認
	//行選択が異常ルートで変更された場合
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_JumpSetup_RegExp);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kColor_JumpSetup_LineColor);
	//データ確認（ContentView展開）
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("33") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("11") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("22") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	//ContentViewが異常ルートで変更された場合
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,kIndex_Invalid);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,kIndex_Invalid);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).NVI_UpdateProperty();
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	//データ確認（ContentView展開）
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("33") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("11") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("22") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	
	//順序ボタン
	//↓
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_JumpSetup_Down,0);//
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("22") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("11") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("33") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	//↑
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_JumpSetup_Up,0);//
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("22") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("33") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("11") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	//bottom
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_JumpSetup_Bottom,0);//
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("33") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("11") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("22") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	//top
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_JumpSetup_Top,0);//
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("11") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("33") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Blue) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,2);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("22") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	//削除
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_JumpSetup_Remove,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_JumpSetup_RemoveOK,0);//削除OK
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("11") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("22") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != true )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Green) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kJumpSetup_Name) != 2 )   _ACThrow("",NULL);
	//削除 B0406 ただし、見出し設定は保存確認ダイアログ表示無しなので、あまりテストする意味はない
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,1);//テーブル選択 #205
	text.SetCstring("aaaaa");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_JumpSetup_RegExp,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_JumpSetup_Remove,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_JumpSetup_RemoveOK,0);//削除OK
	//
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_JumpSetup,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_JumpSetup,0);//テーブル選択 #205
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_JumpSetup,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_RegExp,text);
	if( text.Compare("regexp1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_MenuText,text);
	if( text.Compare("menutext1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_SelectionGroup,text);
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_JumpSetup_KeywordGroup,text);
	if( text.Compare("11") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterToMenu) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_ColorizeLine) != false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_JumpSetup_RegisterAsKeyword) != false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_LineColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlColor(kColor_JumpSetup_KeywordColor,color);
	if( AColorWrapper::CompareColor(color,kColor_Red) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kJumpSetup_Name) != 1 )   _ACThrow("",NULL);
	
	
	
	//========================================
	//   ツール
	//========================================
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SelectTabControl(11);//ツールタブ
	
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kToolMode,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kToolMode,0);
	
	//全削除
	GetApp().SPI_GetModePrefDB(modeIndex).LoadTool();//ツールの一番上の行に「ユーザー作成ツール」のstatictextではなく、testscript1ツールがきていることがあるので、ここで順番を正しくする。でないと下で無限ループする。
	while(GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(GetApp().SPI_GetModePrefDB(modeIndex).GetToolMenuRootObjectID())>1)
	{
		GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
		GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
		GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolRemoveOK,0);//
	}
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolAdd,0);//
	text.SetCstring("Tool1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);
	text.SetCstring("tool content 1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	text.SetCstring("1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool_Shortcut,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool_Shortcut);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutControlKey,0);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutShiftKey,0);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_AddToolFolder,0);//
	text.SetCstring("Tool2");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolAdd,0);//
	text.SetCstring("Tool3");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);
	text.SetCstring("tool content 3");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	text.SetCstring("3");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool_Shortcut,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool_Shortcut);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutControlKey,0);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutShiftKey,0);
	//2の中
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_DoubleClicked(kListView_Tool);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolAdd,0);//
	text.SetCstring("Tool2-1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);
	text.SetCstring("tool content 2-1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	text.SetCstring("A");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool_Shortcut,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool_Shortcut);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutControlKey,0);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutShiftKey,0);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolAdd,0);//
	text.SetCstring("Tool2-2");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);
	text.SetCstring("tool content 2-2");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	text.SetCstring("B");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool_Shortcut,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool_Shortcut);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutControlKey,0);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutShiftKey,0);
	//B0303-1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolParentFolder,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(3);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//2の中
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_DoubleClicked(kListView_Tool);
	//2-1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("A") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2-2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("B") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	
	//異常ルートテスト
	//不具合があっても既存のデータは破壊・消去されないことの確認
	//行選択が異常ルートで変更された場合
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kCheck_Tool_ShortcutControlKey);
	//2-1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("A") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2-2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("B") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	
	//B0303-2 ウインドウ閉じて開く
	GetApp().SPI_GetModePrefWindow(0).EVT_DoCloseButton();
	GetApp().SPI_ShowModePrefWindow(modeIndex);
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(3);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//2の中
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_DoubleClicked(kListView_Tool);
	//2-1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("A") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2-2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("B") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//B0303-3 モード切替
	text.SetText("%%%UnitTest2%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex() != kIndex_Invalid )   _ACThrow("",NULL);
	text.SetText("%%%UnitTest%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex() != kIndex_Invalid )   _ACThrow("",NULL);
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(3);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//2の中
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_DoubleClicked(kListView_Tool);
	//2-1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("A") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2-2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("B") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//ツール・ツールバー切替
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kToolbarMode,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kToolbarMode,0);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kToolMode,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kToolMode,0);
	//2-1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("A") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2-2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("B") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolParentFolder,0);
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(3);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//2の中
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_DoubleClicked(kListView_Tool);
	
	//ファイル内容確認
	AFileAcc	folder;
	GetApp().SPI_GetModePrefDB(modeIndex).GetToolFolder(false,folder);
	file.SpecifyChild(folder,"Tool1");
	file.ReadTo(text);
	if( text.Compare("tool content 1") == false )   _ACThrow("",NULL);
	file.SpecifyChild(folder,"Tool3");
	file.ReadTo(text);
	if( text.Compare("tool content 3") == false )   _ACThrow("",NULL);
	file.SpecifyChild(folder,"Tool2/Tool2-1");
	file.ReadTo(text);
	if( text.Compare("tool content 2-1") == false )   _ACThrow("",NULL);
	file.SpecifyChild(folder,"Tool2/Tool2-2");
	file.ReadTo(text);
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	
	//メニュー内容確認
	//
	AObjectID	objectID = GetApp().SPI_GetModePrefDB(modeIndex).GetToolMenuRootObjectID();
	GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(objectID,1,text);
	if( text.Compare("Tool1") == false )   _ACThrow("",NULL);
	GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(objectID,1,text);
	file.Specify(text);
	file.ReadTo(text);
	if( text.Compare("tool content 1") == false )   _ACThrow("",NULL);
	//
	GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(objectID,3,text);
	if( text.Compare("Tool3") == false )   _ACThrow("",NULL);
	GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(objectID,3,text);
	file.Specify(text);
	file.ReadTo(text);
	if( text.Compare("tool content 3") == false )   _ACThrow("",NULL);
	//
	objectID = GetApp().NVI_GetMenuManager().GetDynamicMenuItemSubMenuObjectIDByObjectID(objectID,2);
	GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(objectID,0,text);
	if( text.Compare("Tool2-1") == false )   _ACThrow("",NULL);
	GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(objectID,0,text);
	file.Specify(text);
	file.ReadTo(text);
	if( text.Compare("tool content 2-1") == false )   _ACThrow("",NULL);
	//
	GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByObjectID(objectID,1,text);
	if( text.Compare("Tool2-2") == false )   _ACThrow("",NULL);
	GetApp().NVI_GetMenuManager().GetDynamicMenuItemActionTextByObjectID(objectID,1,text);
	file.Specify(text);
	file.ReadTo(text);
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	
	//ファイルDropテスト
	file.SpecifyChild(testfolder,"testtoolfile");
	file.CreateFile();
	text.SetCstring("12345");
	file.WriteFile(text);
	GetApp().SPI_GetModePrefWindow(0).OWICB_ListViewFileDropped(kListView_Tool,0,file);
	
	//データ確認（ContentView展開）
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolParentFolder,0);
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(3);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//2の中
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_DoubleClicked(kListView_Tool);
	//dropしたファイル
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("testtoolfile") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("12345") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//2-1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("A") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2-2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("B") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//Dropしたツールの削除
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolRemove,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolRemoveOK,0);//削除OK
	
	
	//Drag&Dropテスト
	//★あとまわし
	
	//未保存状態
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_GetEnableDragDrop() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("aaaaa");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_AddToolFolder,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolAdd,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ReconfigWithToolFolder,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kToolbarMode,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kToolbarMode,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kToolbarMode) != false )   _ACThrow("",NULL);
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_GetEnableDragDrop() == true )   _ACThrow("",NULL);
	//B0406 未保存削除
	//2-1削除
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolRemove,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolRemoveOK,0);//削除OK
	//2-2→2-1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("B") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//データ確認（ContentView展開）
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolParentFolder,0);
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(3);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//2の中
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_DoubleClicked(kListView_Tool);
	//2-1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("B") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolParentFolder,0);
	//ファイル名変更
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("AAAAA");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);//
	//フォルダ名変更
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("This is a test");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);//
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("AAAAA") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("This is a test") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(3);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	
	//B0417
	//ファイル名変更
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("/ hello / world /");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);//
	text.SetCstring("あ");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	text.SetCstring("X");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool_Shortcut,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool_Shortcut);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutControlKey,0);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutShiftKey,0);
	//フォルダ名変更
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("/This is /a/ test/");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);//
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("/ hello / world /") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("あ") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("X") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("/This is /a/ test/") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(3);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	
	//2の中
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_DoubleClicked(kListView_Tool);
	//2-1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("B") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolParentFolder,0);
	//ファイル名変更
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("Tool1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);//
	text.SetCstring("tool content 1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	text.SetCstring("1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool_Shortcut,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool_Shortcut);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutControlKey,0);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Tool_ShortcutShiftKey,0);
	//フォルダ名変更
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("Tool2");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);//
	
	/*
	//ツールバー
	
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kToolbarMode,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kToolbarMode,0);
	
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItemCount()>0)
	{
		GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
		GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
		GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolRemoveOK,0);//
	}
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolAdd,0);//
	text.SetCstring("Toolbar1");
	GetApp().SPI_GetModePrefWindow(0).SPI_GetAddToolButtonWindow().NVI_SetControlText(5200,text);
	GetApp().SPI_GetModePrefWindow(0).SPI_GetAddToolButtonWindow().EVT_Clicked(5001,0);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("toolbar content 1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kButton_Tool_Display,true);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_JumpSetup_RegisterAsKeyword,0);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolAdd,0);//
	text.SetCstring("Toolbar2");
	GetApp().SPI_GetModePrefWindow(0).SPI_GetAddToolButtonWindow().NVI_SetControlText(5200,text);
	GetApp().SPI_GetModePrefWindow(0).SPI_GetAddToolButtonWindow().EVT_Clicked(5001,0);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("toolbar content 2");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kButton_Tool_Display,false);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_Tool_Display,0);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolAdd,0);//
	text.SetCstring("Toolbar3");
	GetApp().SPI_GetModePrefWindow(0).SPI_GetAddToolButtonWindow().NVI_SetControlText(5200,text);
	GetApp().SPI_GetModePrefWindow(0).SPI_GetAddToolButtonWindow().EVT_Clicked(5001,0);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("toolbar content 3");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kButton_Tool_Display,true);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_Tool_Display,0);
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 2") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == false )   _ACThrow("",NULL);
	
	//ツール・ツールバー切替
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kToolMode,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kToolMode,0);
	//ツールデータ確認
	//データ確認（ContentView展開）
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolParentFolder,0);
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(3);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == true )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == true )   _ACThrow("",NULL);
	//2の中
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_DoubleClicked(kListView_Tool);
	//2-1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Tool2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("tool content 2-2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool_Shortcut,text);//
	if( text.Compare("B") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutControlKey) == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kCheck_Tool_ShortcutShiftKey) == false )   _ACThrow("",NULL);
	//ツールバーモード
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kToolbarMode,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kToolbarMode,0);
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 2") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == false )   _ACThrow("",NULL);
	*/
	
	//B0303-3 モード切替
	text.SetText("%%%UnitTest2%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex() != kIndex_Invalid )   _ACThrow("",NULL);
	text.SetText("%%%UnitTest%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_GetSelectedDBIndex() != kIndex_Invalid )   _ACThrow("",NULL);
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 2") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == false )   _ACThrow("",NULL);
	
	//B0303-2 ウインドウ閉じて開く
	GetApp().SPI_GetModePrefWindow(0).EVT_DoCloseButton();
	GetApp().SPI_ShowModePrefWindow(modeIndex);
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 1") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == false )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 2") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == true )   _ACThrow("",NULL);
	//3
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(2);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == false )   _ACThrow("",NULL);
	
	//ファイル内容確認
	GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarFolder(/*#844 false,*/folder);
	file.SpecifyChild(folder,"Toolbar1");
	file.ReadTo(text);
	if( text.Compare("toolbar content 1") == false )   _ACThrow("",NULL);
	file.SpecifyChild(folder,"Toolbar2");
	file.ReadTo(text);
	if( text.Compare("toolbar content 2") == false )   _ACThrow("",NULL);
	file.SpecifyChild(folder,"Toolbar3");
	file.ReadTo(text);
	if( text.Compare("toolbar content 3") == false )   _ACThrow("",NULL);
	
	//未保存状態
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("tttttt");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_AddToolFolder,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolAdd,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ReconfigWithToolFolder,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kToolMode,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kToolMode,0);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kToolMode) != false )   _ACThrow("",NULL);
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	if( GetApp().SPI_GetModePrefWindow(0).NVI_IsChildWindowVisible() == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_GetEnableDragDrop() == true )   _ACThrow("",NULL);
	//B0406 未保存削除
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolRemove,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolRemoveOK,0);//削除OK
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 2") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == true )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == false )   _ACThrow("",NULL);
	
	//B0417
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("/ hello / world /");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);//
	text.SetCstring("森鷗外");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("/ hello / world /") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("森鷗外") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == true )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == false )   _ACThrow("",NULL);
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	text.SetCstring("Toolbar2");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_ToolName);//
	text.SetCstring("toolbar content 2");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_ToolSave,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	//データ確認（ContentView展開）
	//1
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 2") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == true )   _ACThrow("",NULL);
	//2
	GetApp().SPI_GetModePrefWindow(0).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kListView_Tool,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_ToolName,text);//
	if( text.Compare("Toolbar3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_Tool,text);//
	if( text.Compare("toolbar content 3") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefWindow(0).NVI_GetControlBool(kButton_Tool_Display) == false )   _ACThrow("",NULL);
	
	
	//========================================
	//   Drag&Drop
	//========================================
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SelectTabControl(8);//入力支援タブ
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kDragDrop_Suffix)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kDragDrop_Suffix,0);
	}
	//全削除 "%%%UnitTest2%%%"のほう
	while(GetApp().SPI_GetModePrefDB(modeIndex2).GetItemCount_Array(AModePrefDB::kDragDrop_Suffix)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex2).DeleteRow_Table(AModePrefDB::kDragDrop_Suffix,0);
	}
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_DragDropAdd,0);//
	text.SetCstring("<<<PATH>>>\r*****\r****\r***\r**\r*\r");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_DragDrapText,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_DragDrapText);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_DragDropAdd,0);//
	text.SetCstring("*****\r****\r***\r**\r*\r");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_DragDrapText,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_DragDrapText);
	//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_DragDropAdd,0);//
	text.SetCstring("draganddrop3");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_DragDrapText,text);//
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_DragDrapText);
	//拡張子
	ta.DeleteAll();
	ta.Add(".sfx1");
	ta.Add(".sfx2");
	ta.Add(".sfx3");
	/*#205 GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().BeginSetTable(kTable_DragDrop);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().SetTable_Text(kTable_DragDrop,0,ta);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().EndSetTable(kTable_DragDrop);*/
	GetApp().SPI_GetModePrefWindow(0).NVI_BeginSetTable(kTable_DragDrop);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetTableColumn_Text(kTable_DragDrop,0,ta);
	GetApp().SPI_GetModePrefWindow(0).NVI_EndSetTable(kTable_DragDrop);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kTable_DragDrop);
	//データ確認
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kDragDrop_Suffix,0,text);
	if( text.Compare(".sfx1") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kDragDrop_Text,0,text);
	if( text.Compare("<<<PATH>>>\r*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kDragDrop_Suffix,1,text);
	if( text.Compare(".sfx2") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kDragDrop_Text,1,text);
	if( text.Compare("*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kDragDrop_Suffix,2,text);
	if( text.Compare(".sfx3") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kDragDrop_Text,2,text);
	if( text.Compare("draganddrop3") == false )   _ACThrow("",NULL);
	//データ確認（ContentView展開）
	//1
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("<<<PATH>>>\r*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	//2
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,1);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	//3
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,2);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("draganddrop3") == false )   _ACThrow("",NULL);
	
	//モード切替
	text.SetText("%%%UnitTest2%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0)./*#205 NVI_GetControlNumber*/NVI_GetTableViewSelectionDBIndex(kTable_DragDrop) != kIndex_Invalid )   _ACThrow("",NULL);
	text.SetText("%%%UnitTest%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0)./*#205 NVI_GetControlNumber*/NVI_GetTableViewSelectionDBIndex(kTable_DragDrop) != kIndex_Invalid )   _ACThrow("",NULL);
	//データ確認（ContentView展開）
	//1
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("<<<PATH>>>\r*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	//2
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,1);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	//3
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,2);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("draganddrop3") == false )   _ACThrow("",NULL);
	
	//選択中に閉じて開いてみる
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_KeywordCategory,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_KeywordCategory,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_DoCloseButton();
	GetApp().SPI_ShowModePrefWindow(modeIndex);
	//データ確認（ContentView展開）
	//1
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("<<<PATH>>>\r*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	//2
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,1);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	//3
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,2);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("draganddrop3") == false )   _ACThrow("",NULL);
	
	//エクスポート・インポート
	file.SpecifyChild(testfolder,"DragAndDropExportTest.txt");
	file.CreateFile();
	text.SetCstring("dragdropexport");
	GetApp().SPI_GetModePrefWindow(0).EVT_SaveWindowReply(kObjectID_Invalid,file,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_FileChoosen(kButton_DragDropImport,file);
	//データ確認（ContentView展開）
	//1
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("<<<PATH>>>\r*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	//2
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,1);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	//3
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,2);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("draganddrop3") == false )   _ACThrow("",NULL);
	
	//削除
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_DragDrapRemove,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_DragDrapRemoveOK,0);//削除OK
	//データ確認（ContentView展開）
	//2
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	//3
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,1);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("draganddrop3") == false )   _ACThrow("",NULL);
	
	//インポート
	file.SpecifyChild(testfolder,"DragAndDropExportTest.txt");
	file.CreateFile();
	GetApp().SPI_GetModePrefWindow(0).EVT_FileChoosen(kButton_DragDropImport,file);
	//データ確認（ContentView展開）
	//1
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("<<<PATH>>>\r*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	//2
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,1);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("*****\r****\r***\r**\r*\r") == false )   _ACThrow("",NULL);
	//3
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_DragDrop,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_DragDrop,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_DragDrop,2);//
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_DragDrapText,text);
	if( text.Compare("draganddrop3") == false )   _ACThrow("",NULL);
	
	//========================================
	//   対応文字列
	//========================================
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SelectTabControl(8);//入力支援タブ
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kCorrespond_StartText)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kCorrespond_StartText,0);
	}
	//全削除 "%%%UnitTest2%%%"のほう
	while(GetApp().SPI_GetModePrefDB(modeIndex2).GetItemCount_Array(AModePrefDB::kCorrespond_StartText)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex2).DeleteRow_Table(AModePrefDB::kCorrespond_StartText,0);
	}
	//
	ATextArray	ta1, ta2;
	for( AIndex i = 0; i < 4096; i++ )
	{
		text.SetFormattedCstring("start%d",i);
		ta1.Add(text);
		text.SetFormattedCstring("end%d",i);
		ta2.Add(text);
	}
	/*#205 GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().BeginSetTable(kTable_Correspond);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().SetTable_Text(kTable_Correspond,0,ta1);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().SetTable_Text(kTable_Correspond,1,ta2);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().EndSetTable(kTable_Correspond);*/
	GetApp().SPI_GetModePrefWindow(0).NVI_BeginSetTable(kTable_Correspond);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetTableColumn_Text(kTable_Correspond,0,ta1);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetTableColumn_Text(kTable_Correspond,1,ta2);
	GetApp().SPI_GetModePrefWindow(0).NVI_EndSetTable(kTable_Correspond);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kTable_Correspond);
	//エクスポート・インポート
	file.SpecifyChild(testfolder,"CorrespondExportTest.txt");
	file.CreateFile();
	text.SetCstring("correspondexport");
	GetApp().SPI_GetModePrefWindow(0).EVT_SaveWindowReply(kObjectID_Invalid,file,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_FileChoosen(kButton_CorrespondImport,file);
	//データ確認
	for( AIndex i = 0; i < 4096; i++ )
	{
		AText	t;
		GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kCorrespond_StartText,i,t);
		text.SetFormattedCstring("start%d",i);
		if( t.Compare(text) == false )   _ACThrow("",NULL);
		GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kCorrespond_EndText,i,t);
		text.SetFormattedCstring("end%d",i);
		if( t.Compare(text) == false )   _ACThrow("",NULL);
	}
	//削除
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Correspond,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Correspond,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Correspond,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_CorrespondRemove,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_CorrespondRemoveOK,0);//削除OK
	//データ確認
	for( AIndex i = 1; i < 4096; i++ )
	{
		AText	t;
		GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kCorrespond_StartText,i-1,t);
		text.SetFormattedCstring("start%d",i);
		if( t.Compare(text) == false )   _ACThrow("",NULL);
		GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kCorrespond_EndText,i-1,t);
		text.SetFormattedCstring("end%d",i);
		if( t.Compare(text) == false )   _ACThrow("",NULL);
	}
	//インポート
	file.SpecifyChild(testfolder,"CorrespondExportTest.txt");
	GetApp().SPI_GetModePrefWindow(0).EVT_FileChoosen(kButton_CorrespondImport,file);
	//データ確認
	for( AIndex i = 0; i < 4096; i++ )
	{
		AText	t;
		GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kCorrespond_StartText,i,t);
		text.SetFormattedCstring("start%d",i);
		if( t.Compare(text) == false )   _ACThrow("",NULL);
		GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kCorrespond_EndText,i,t);
		text.SetFormattedCstring("end%d",i);
		if( t.Compare(text) == false )   _ACThrow("",NULL);
	}
	
	//========================================
	//   インデント
	//========================================
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SelectTabControl(9);//インデントタブ
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kIndent_RegExp)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kIndent_RegExp,0);
	}
	//全削除 "%%%UnitTest2%%%"のほう
	while(GetApp().SPI_GetModePrefDB(modeIndex2).GetItemCount_Array(AModePrefDB::kIndent_RegExp)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex2).DeleteRow_Table(AModePrefDB::kIndent_RegExp,0);
	}
	ta.DeleteAll();
	text.SetCstring(".*<p[ >]");
	ta.Add(text);
	text.SetCstring(".*</p>");
	ta.Add(text);
	ANumberArray	na1, na2;
	na1.Add(0);
	na1.Add(-4);
	na2.Add(4);
	na2.Add(0);
	/*#205 GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().BeginSetTable(kTable_Indent);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().SetTable_Text(kTable_Indent,0,ta);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().SetTable_Number(kTable_Indent,1,na1);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().SetTable_Number(kTable_Indent,2,na2);
	GetApp().SPI_GetModePrefWindow(0).NVM_GetImp().EndSetTable(kTable_Indent);*/
	GetApp().SPI_GetModePrefWindow(0).NVI_BeginSetTable(kTable_Indent);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetTableColumn_Text(kTable_Indent,0,ta);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetTableColumn_Number(kTable_Indent,1,na1);
	GetApp().SPI_GetModePrefWindow(0).NVI_SetTableColumn_Number(kTable_Indent,2,na2);
	GetApp().SPI_GetModePrefWindow(0).NVI_EndSetTable(kTable_Indent);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kTable_Indent);
	//エクスポート・インポート
	file.SpecifyChild(testfolder,"RegExpIndentExportTest.txt");
	file.CreateFile();
	text.SetCstring("regexpindentexport");
	GetApp().SPI_GetModePrefWindow(0).EVT_SaveWindowReply(kObjectID_Invalid,file,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_FileChoosen(kButton_RegExpIndentImport,file);
	//データ確認
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kIndent_RegExp,0,text);
	if( text.Compare(".*<p[ >]") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kIndent_RegExp,1,text);
	if( text.Compare(".*</p>") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetCurrentLine,0) != 0 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetNextLine,0) != 4 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetCurrentLine,1) != -4 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetNextLine,1) != 0 )   _ACThrow("",NULL);
	//削除
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Indent,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Indent,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Indent,0);//
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_IndentRemove,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_CloseChildWindow();
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_IndentRemoveOK,0);//削除OK
	//データ確認
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kIndent_RegExp,0,text);
	if( text.Compare(".*</p>") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetCurrentLine,0) != -4 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetNextLine,0) != 0 )   _ACThrow("",NULL);
	//インポート
	file.SpecifyChild(testfolder,"RegExpIndentExportTest.txt");
	GetApp().SPI_GetModePrefWindow(0).EVT_FileChoosen(kButton_RegExpIndentImport,file);
	//データ確認
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kIndent_RegExp,0,text);
	if( text.Compare(".*<p[ >]") == false )   _ACThrow("",NULL);
	GetApp().SPI_GetModePrefDB(modeIndex).GetData_TextArray(AModePrefDB::kIndent_RegExp,1,text);
	if( text.Compare(".*</p>") == false )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetCurrentLine,0) != 0 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetNextLine,0) != 4 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetCurrentLine,1) != -4 )   _ACThrow("",NULL);
	if( GetApp().SPI_GetModePrefDB(modeIndex).GetData_NumberArray(AModePrefDB::kIndent_OffsetNextLine,1) != 0 )   _ACThrow("",NULL);
	
	
	//========================================
	//   キーバインド
	//========================================
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SelectTabControl(10);//キーバインドタブ
	//全削除
	while( GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_NumberArray(AModePrefDB::kKeyBind_Key) > 0 )
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kKeyBind_Key,0);
	}
	//全削除
	while( GetApp().SPI_GetModePrefDB(modeIndex2).GetItemCount_NumberArray(AModePrefDB::kKeyBind_Key) > 0 )
	{
		GetApp().SPI_GetModePrefDB(modeIndex2).DeleteRow_Table(AModePrefDB::kKeyBind_Key,0);
	}
	//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Keybind_ControlKey,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Keybind_ControlKey,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Keybind_OptionKey,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Keybind_OptionKey,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Keybind_CommandKey,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Keybind_CommandKey,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Keybind_ShiftKey,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Keybind_ShiftKey,0);//
	//ctrl-A
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,0);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kPopup_KeybindAction,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kPopup_KeybindAction);
	text.SetCstring("ctrl-A\r*****\r****\r***\r**\r*");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeybindActionText,text);
	//ctrl-B
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,0);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kPopup_KeybindAction,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kPopup_KeybindAction);
	text.SetCstring("ctrl-B");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeybindActionText,text);
	//ctrl-C
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,0);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kPopup_KeybindAction,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kPopup_KeybindAction);
	text.SetCstring("ctrl-C");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeybindActionText,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kButton_KeyBindTextOK,0);
	//
	/*実際にドキュメントに入力して試してみる（あとまわし）
	AObjectID	docID = GetApp().SPNVI_CreateNewTextDocument(modeIndex);
	AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
	AControlID	textViewID = GetApp().SPI_GetTextWindowByID(winID).NVI_GetCurrentFocus();
	AView_Text&	textView = dynamic_cast<AView_Text&>(GetApp().SPI_GetTextWindowByID(winID).NVI_GetViewByControlID(textViewID));
	textView.EVTDO_DoTextInput()*/
	//データ確認(ContentView展開）
	//ctrl-A
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,0);//
	text.SetLocalizedText("Keybind_action1");
	AText	t;
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("ctrl-A\r*****\r****\r***\r**\r*");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	//ctrl-B
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,1);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("ctrl-B");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	//ctrl-C
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,2);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("ctrl-C");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	
	//エクスポート・インポート
	file.SpecifyChild(testfolder,"KeybindExportTest.txt");
	file.CreateFile();
	text.SetCstring("keybindexport");
	GetApp().SPI_GetModePrefWindow(0).EVT_SaveWindowReply(kObjectID_Invalid,file,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_FileChoosen(kButton_Keybind_Import,file);
	//データ確認(ContentView展開）
	//ctrl-A
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,0);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("ctrl-A\r*****\r****\r***\r**\r*");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	//ctrl-B
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,1);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("ctrl-B");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	//ctrl-C
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,2);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("ctrl-C");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	
	//文字列変更→インポート
	//ctrl-A
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,0);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kPopup_KeybindAction,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kPopup_KeybindAction);
	text.SetCstring("aaa");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeybindActionText,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kText_KeybindActionText);
	//ctrlキーチェックボックスOff,On
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Keybind_ControlKey,false);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Keybind_ControlKey,0);//
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlBool(kCheck_Keybind_ControlKey,true);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kCheck_Keybind_ControlKey,0);//
	//ctrl-B
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,0);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kPopup_KeybindAction,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kPopup_KeybindAction);
	text.SetCstring("bbb");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeybindActionText,text);
	//閉じて開く
	GetApp().SPI_GetModePrefWindow(0).EVT_DoCloseButton();
	GetApp().SPI_ShowModePrefWindow(modeIndex);
	//ctrl-C
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,0);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kPopup_KeybindAction,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_ValueChanged(kPopup_KeybindAction);
	text.SetCstring("ccc");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kText_KeybindActionText,text);
	//モード切替
	text.SetText("%%%UnitTest2%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0)./*#205 NVI_GetControlNumber*/NVI_GetTableViewSelectionDBIndex(kTable_Keybind) != kIndex_Invalid )   _ACThrow("",NULL);
	text.SetText("%%%UnitTest%%%");
	GetApp().SPI_GetModePrefWindow(0).NVI_SetControlText(kMenu_ModeSelect,text);
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kMenu_ModeSelect,0);//
	if( GetApp().SPI_GetModePrefWindow(0)./*#205 NVI_GetControlNumber*/NVI_GetTableViewSelectionDBIndex(kTable_Keybind) != kIndex_Invalid )   _ACThrow("",NULL);
	//データ確認(ContentView展開）
	//ctrl-A
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,0);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("aaa");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	//ctrl-B
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,1);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("bbb");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	//ctrl-C
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,2);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("ccc");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	
	//インポート
	file.SpecifyChild(testfolder,"KeybindExportTest.txt");
	GetApp().SPI_GetModePrefWindow(0).EVT_FileChoosen(kButton_Keybind_Import,file);
	//データ確認(ContentView展開）
	//ctrl-A
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,0);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,0);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("ctrl-A\r*****\r****\r***\r**\r*");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	//ctrl-B
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,1);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,1);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("ctrl-B");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	//ctrl-C
	//#205 GetApp().SPI_GetModePrefWindow(0).NVI_SetControlNumber(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).NVM_SelectTableRow(kTable_Keybind,2);//テーブル選択
	GetApp().SPI_GetModePrefWindow(0).EVT_Clicked(kTable_Keybind,2);//
	text.SetLocalizedText("Keybind_action1");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kPopup_KeybindAction,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	text.SetCstring("ctrl-C");
	GetApp().SPI_GetModePrefWindow(0).NVI_GetControlText(kText_KeybindActionText,t);
	if( text.Compare(t) == false )   _ACThrow("",NULL);
	
	//GetApp().SPI_GetModePrefWindow(0).NVI_Hide();
//#205 #endif
	return result;
}

//B0408
ABool	TransliterateTest()
{
	ABool	result = true;
	
	AFileAcc	testfolder;
	AFileWrapper::GetLocalizedResourceFile("TestPatterns",testfolder);
	//元ファイル読み込み
	AFileAcc	origfile;
	origfile.SpecifyChild(testfolder,"trans_test_orig");
	AText	origtext;
	origfile.ReadTo(origtext);
	//半角ｶﾅ→全角カナ
	AText	text;
	AText	title;
	title.SetCstring("半角ｶﾅ→全角カナ");
	if( GetApp().SPI_Transliterate(title,origtext,text) == false )   _ACThrow("",NULL);
	//確認
	AFileAcc	cmpfile;
	cmpfile.SpecifyChild(testfolder,"trans_test_hankana_to_zenkana");
	AText	cmptext;
	cmpfile.ReadTo(cmptext);
	if( cmptext.Compare(text) == false )   _ACThrow("",NULL);
	//全角カナ→半角ｶﾅ
	AText	text2;
	title.SetCstring("全角カナ→半角ｶﾅ");
	if( GetApp().SPI_Transliterate(title,text,text2) == false )   _ACThrow("",NULL);
	//確認
	if( text2.Compare(origtext) == false )   _ACThrow("",NULL);
	
	//半角英数字→全角英数字
	title.SetCstring("半角英数(A-z,0-9)→全角英数(Ａ-ｚ,０-９)");
	if( GetApp().SPI_Transliterate(title,origtext,text) == false )   _ACThrow("",NULL);
	//確認
	cmpfile.SpecifyChild(testfolder,"trans_test_haneisu_to_zeneisu");
	cmpfile.ReadTo(cmptext);
	if( cmptext.Compare(text) == false )   _ACThrow("",NULL);
	title.SetCstring("全角英数(Ａ-ｚ,０-９)→半角英数(A-z,0-9)");
	if( GetApp().SPI_Transliterate(title,text,text2) == false )   _ACThrow("",NULL);
	//確認
	if( text2.Compare(origtext) == false )   _ACThrow("",NULL);
	
	//半角英数字→全角英数字 #55
	title.SetCstring("半角英数記号(A-z,0-9,!,...)→全角英数記号(Ａ-ｚ,０-９,！,...)");
	if( GetApp().SPI_Transliterate(title,origtext,text) == false )   _ACThrow("",NULL);
	//確認
	cmpfile.SpecifyChild(testfolder,"trans_test_haneisukigou_to_zeneisukigou");
	cmpfile.ReadTo(cmptext);
	if( cmptext.Compare(text) == false )   _ACThrow("",NULL);
	title.SetCstring("全角英数記号(Ａ-ｚ,０-９,！,...)→半角英数記号(A-z,0-9,!,...)");
	if( GetApp().SPI_Transliterate(title,text,text2) == false )   _ACThrow("",NULL);
	//確認
	if( text2.Compare(origtext) == false )   _ACThrow("",NULL);
	
	//半角スペース→全角スペース #68
	title.SetCstring("半角スペース→全角スペース");
	if( GetApp().SPI_Transliterate(title,origtext,text) == false )   _ACThrow("",NULL);
	//確認
	cmpfile.SpecifyChild(testfolder,"trans_test_hanspace_to_zenspace");
	cmpfile.ReadTo(cmptext);
	if( cmptext.Compare(text) == false )   _ACThrow("",NULL);
	title.SetCstring("全角スペース→半角スペース");
	if( GetApp().SPI_Transliterate(title,text,text2) == false )   _ACThrow("",NULL);
	//確認
	if( text2.Compare(origtext) == false )   _ACThrow("",NULL);
	
	//全角カナ→全角かな #68
	AFileAcc	origfile2;
	origfile2.SpecifyChild(testfolder,"trans_test_orig2");
	AText	origtext2;
	origfile2.ReadTo(origtext2);
	//
	title.SetCstring("全角カナ→全角かな");
	if( GetApp().SPI_Transliterate(title,origtext2,text) == false )   _ACThrow("",NULL);
	//確認
	cmpfile.SpecifyChild(testfolder,"trans_test_zenkana_to_zengana");
	cmpfile.ReadTo(cmptext);
	if( cmptext.Compare(text) == false )   _ACThrow("",NULL);
	title.SetCstring("全角かな→全角カナ");
	if( GetApp().SPI_Transliterate(title,text,text2) == false )   _ACThrow("",NULL);
	//確認
	if( text2.Compare(origtext2) == false )   _ACThrow("",NULL);
	
	return result;
}

ABool	AppleScriptTest()
{
	ABool	result = true;
	
	AFileAcc	testfolder;
	AFileWrapper::GetLocalizedResourceFile("TestPatterns",testfolder);
	AFileAcc	file;
	file.SpecifyChild(testfolder,"AppleScriptTest.scpt");
	
	GetApp().SPI_ExecuteAppleScript(file);
	
	
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	ADocument_Text&	doc = GetApp().SPI_GetTextDocumentByID(docID);
	AText	text;
	doc.SPI_GetText(text);
	if( text.Compare("AppleScriptTest completed.") == false )   result = false;
	
	//#379 doc.SPI_ExecuteClose();
	GetApp().SPI_ExecuteCloseTextDocument(docID);
	return result;
}

//#567
ABool	LuaTest()
{
	AFileAcc	testfolder;
	AFileWrapper::GetLocalizedResourceFile("TestPatterns",testfolder);
	AFileAcc	file;
	file.SpecifyChild(testfolder,"test.lua");
	
	AText	errorText;
	if( GetApp().NVI_Lua_dofile(file,errorText) != 0 )
	{
		return false;
	}
	
	AObjectID	docID = GetApp().NVI_GetMostFrontDocumentID(kDocumentType_Text);
	ADocument_Text&	doc = GetApp().SPI_GetTextDocumentByID(docID);
	AText	text;
	doc.SPI_GetText(text);
	if( text.Compare("lua auto test code completed.") == false )
	{
		return false;
	}
	
	GetApp().SPI_ExecuteCloseTextDocument(docID);
	return true;
}

//DocumentTest用のモード設定
void	DocumentTest_ModeSetup()
{
	AFileAcc	testpatfolder;
	AFileWrapper::GetLocalizedResourceFile("TestPatterns",testpatfolder);
	
	AText	text("%%%UnitTest%%%");
	AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(text);
	if( modeIndex == kIndex_Invalid )
	{
		modeIndex = GetApp().SPI_AddNewMode(text);
	}
	
	GetApp().SPI_ShowModePrefWindow(modeIndex);
	
	//========================================
	//   ツール
	//========================================
	//
	GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SelectTabControl(9);//ツールタブ
	
	//
	//#688 GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SetControlBool(kToolMode,true);
	//#688 GetApp().SPI_GetModePrefWindow(modeIndex).EVT_Clicked(kToolMode,0);
	
	//全削除
	while(GetApp().NVI_GetMenuManager().GetDynamicMenuItemCountByObjectID(GetApp().SPI_GetModePrefDB(modeIndex).GetToolMenuRootObjectID())>2)
	{
		GetApp().SPI_GetModePrefWindow(modeIndex).NVI_GetListView(kListView_Tool).SPI_SetSelect(1);
		//GetApp().SPI_GetModePrefWindow(modeIndex).EVT_Clicked(kListView_Tool,0);//
		GetApp().SPI_GetModePrefWindow(modeIndex).EVT_Clicked(kButton_ToolRemoveOK,0);//
	}
	
	//tool 1
	//R0137
	ABool	svnocheckmodeexec = GetApp().SPI_IsNoCheckModeExecutable();
	GetApp().SPI_SetNoCheckModeExecutable(true);
	
	AFileAcc	file;
	file.SpecifyChild(testpatfolder,"testscript1.scpt");
	GetApp().SPI_GetModePrefWindow(modeIndex).OWICB_ListViewFileDropped(kListView_Tool,0,file);
	
	//R0137
	GetApp().SPI_SetNoCheckModeExecutable(svnocheckmodeexec);
	
	//tool 2
	GetApp().SPI_GetModePrefWindow(modeIndex).EVT_Clicked(kButton_ToolAdd,0);//
	text.SetCstring("Tool2");
	GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SetControlText(kText_ToolName,text);//
	GetApp().SPI_GetModePrefWindow(modeIndex).EVT_ValueChanged(kText_ToolName);
	text.SetCstring("<<<TOOL(/%%%UnitTest%%%/tool/testscript1.scpt");
	GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SetControlText(kText_Tool,text);//
	GetApp().SPI_GetModePrefWindow(modeIndex).EVT_TextInputted(kText_Tool);
	GetApp().SPI_GetModePrefWindow(modeIndex).EVT_Clicked(kButton_ToolSave,0);//
	text.SetCstring("1");
	GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SetControlText(kText_Tool_Shortcut,text);//
	GetApp().SPI_GetModePrefWindow(modeIndex).EVT_TextInputted(kText_Tool_Shortcut);
	GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SetControlBool(kCheck_Tool_ShortcutControlKey,true);
	GetApp().SPI_GetModePrefWindow(modeIndex).EVT_Clicked(kCheck_Tool_ShortcutControlKey,0);
	GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SetControlBool(kCheck_Tool_ShortcutShiftKey,true);
	GetApp().SPI_GetModePrefWindow(modeIndex).EVT_Clicked(kCheck_Tool_ShortcutShiftKey,0);
	
	/*#724
	//ツールバー
	
	//
	GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SetControlBool(kToolbarMode,true);
	GetApp().SPI_GetModePrefWindow(modeIndex).EVT_Clicked(kToolbarMode,0);
	
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetToolbarItemCount()>0)
	{
		GetApp().SPI_GetModePrefWindow(modeIndex).NVI_GetListView(kListView_Tool).SPI_SetSelect(0);
		GetApp().SPI_GetModePrefWindow(modeIndex).EVT_Clicked(kListView_Tool,0);//
		GetApp().SPI_GetModePrefWindow(modeIndex).EVT_Clicked(kButton_ToolRemoveOK,0);//
	}
	*/
	//GetApp().SPI_GetModePrefWindow(modeIndex).EVT_DoCloseButton();
	
	AFileAcc	testfolder;
	testfolder.SpecifyAsUnitTestFolder();
	
	//========================================
	//   キーワード
	//========================================
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kAdditionalCategory_Name)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kAdditionalCategory_Name,0);
		//#427 GetApp().SPI_GetModePrefDB(modeIndex).GetAdditionalCategoryKeywordArray().Delete1Object(0);
	}
	
	//========================================
	//   見出し
	//========================================
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kJumpSetup_Name)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kJumpSetup_Name,0);
	}
	
	//========================================
	//   Drag&Drop
	//========================================
	//
	GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SelectTabControl(7);//入力支援タブ
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kDragDrop_Suffix)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kDragDrop_Suffix,0);
	}
	
	//========================================
	//   対応文字列
	//========================================
	//
	GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SelectTabControl(7);//入力支援タブ
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kCorrespond_StartText)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kCorrespond_StartText,0);
	}
	
	//========================================
	//   インデント
	//========================================
	//
	/* ModePrefWindowTest()で設定した<p></p>のインデント設定を残すことにする
	GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SelectTabControl(9);//インデントタブ
	//全削除
	while(GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_Array(AModePrefDB::kIndent_RegExp)>0)
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kIndent_RegExp,0);
	}
	*/
	
	//========================================
	//   キーバインド
	//========================================
	//
	GetApp().SPI_GetModePrefWindow(modeIndex).NVI_SelectTabControl(8);//キーバインドタブ
	//全削除
	while( GetApp().SPI_GetModePrefDB(modeIndex).GetItemCount_NumberArray(AModePrefDB::kKeyBind_Key) > 0 )
	{
		GetApp().SPI_GetModePrefDB(modeIndex).DeleteRow_Table(AModePrefDB::kKeyBind_Key,0);
	}
	
	//禁則
	GetApp().NVI_GetAppPrefDB().SetData_Bool(AAppPrefDB::kKinsokuBuraasge,false);
}

//行折り返し計算テスト
void	WrapTest1( ADocument_Text& inDocument, AWindow_Text& inWindow, const AText& inText )
{
	//ドキュメント全削除
	inDocument.SPI_DeleteText(0,inDocument.SPI_GetTextLength());
	
	//
	ANumber	width = 1600;
	ARect	bounds;
	bounds.left		= 100;
	bounds.top		= 100;
	bounds.right	= bounds.left+width;
	bounds.bottom	= 900;
	inWindow.NVI_SetWindowBounds(bounds);
	
	//
	ATextPoint	pt = {0,0};
	AText	t(inText);
	inDocument.SPI_InsertText(pt,t);
	
	//テキストデータ取得
	AItemCount	crcount = 0;
	AItemCount	unicharcount = 0;
	for( AIndex i = 0; i < inText.GetItemCount(); i = inText.GetNextCharPos(i) )
	{
		AUChar ch = inText.Get(i);
		if( ch == kUChar_LineEnd )   crcount++;
		unicharcount++;
	}
	
	while( width > 60 )
	{
		//fprintf(stderr,"w:%d ",width);
		bounds.right = bounds.left+width;
		if( inDocument.SPI_GetWrapMode() == kWrapMode_WordWrap )
		{
			inWindow.NVI_SetWindowBounds(bounds);
			//B0415 inWindow.EVT_WindowResized();
		}
		else if( inDocument.SPI_GetWrapMode() == kWrapMode_WordWrapByLetterCount )
		{
			AItemCount	lc = (width-50)/10;
			if( lc <= 0 )   lc = 1;
			inDocument.SPI_SetWrapMode(kWrapMode_WordWrapByLetterCount,lc);
		}
		else
		{
			_ACThrow("",NULL);
		}
		
		//データチェック
		AItemCount	linecount = inDocument.SPI_GetLineCount();
		for( AIndex i = 0; i < linecount; i++ )
		{
			AIndex	spos = inDocument.SPI_GetLineStart(i);
			AItemCount	len = inDocument.SPI_GetLineLengthWithLineEnd(i);
			AIndex	nextspos = inText.GetItemCount();
			if( i+1 < linecount )
			{
				nextspos = inDocument.SPI_GetLineStart(i+1);
			}
			//fprintf(stderr,"%d %d %d   ",spos,len,nextspos);
			if( spos + len != nextspos )   _ACThrow("",NULL);
			if( inDocument.SPI_GetLineExistLineEnd(i) == true )
			{
				if( len != inDocument.SPI_GetLineLengthWithoutLineEnd(i)+1 )   _ACThrow("",NULL);
			}
			else
			{
				if( len != inDocument.SPI_GetLineLengthWithoutLineEnd(i) )   _ACThrow("",NULL);
			}
		}
		//fprintf(stderr,"%d %d   ",inDocument.SPI_GetParagraphCount(),crcount);
		if( inDocument.SPI_GetParagraphCount() != crcount+1 )   _ACThrow("",NULL);
		
		//編集テスト
		//元のstartposを記憶
		AArray<AIndex>	origspos;
		for( AIndex i = 0; i < linecount; i++ )
		{
			origspos.Add(inDocument.SPI_GetLineStart(i));
		}
		
		//各行の行頭に"test"を追加・削除
		AText	text("test");
		ATextPoint	spt;//, ept;
		for( AIndex i = 0; i < linecount; i++ )
		{
			if( linecount > 100 )   break;
			spt.x = 0;
			spt.y = i;
			AIndex	p = inDocument.SPI_GetTextIndexFromTextPoint(spt);
			inDocument.SPI_InsertText(p,text);
			inDocument.SPI_DeleteText(p,p+text.GetItemCount());
		}
		
		//元のstartposと同じかどうか確認
		for( AIndex i = 0; i < linecount; i++ )
		{
			//fprintf(stderr,"%d %d %d   ",i,inDocument.SPI_GetLineStart(i),origspos.Get(i));
			if( inDocument.SPI_GetLineStart(i) != origspos.Get(i) )   _ACThrow("",NULL);
		}
		
		
		//徐々に幅小さく
		if( width > 50 )
		{
			width *= 0.8;
		}
		else
		{
			width -= 5;
		}
	}
}

ABool	WrapTest( ADocument_Text& inDocument, AWindow_Text& inWindow, AView_Text& inView  )
{
	ABool	result = true;
	
	AText	text;
	
	ATextPoint	pt, spt, ept;
	ATextIndex	textpos;
	AUChar	ch;
	
	//==================
	//  行折り返しテスト
	//==================
	
	ARect	svBounds;
	inWindow.NVI_GetWindowBounds(svBounds);
	
	//ドキュメント全削除
	inView.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
	inView.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	
	//
	ARect	bounds;
	bounds.left		= 100;
	bounds.top		= 100;
	bounds.right	= 900;
	bounds.bottom	= 900;
	inWindow.NVI_SetWindowBounds(bounds);
	//B0415 inWindow.EVT_WindowResized();
	
	//B0415 NVI_SetWindowBounds()でEVT_WindowBoundsChanged()がコールされるように変更
	text.DeleteAll();
	for( AIndex i = 0; i < 500; i++ )
	{
		text.AddCstring("あ");
	}
	pt.x = 0;
	pt.y = 0;
	inDocument.SPI_InsertText(pt,text);
	AItemCount	linecount = inDocument.SPI_GetLineCount();
	bounds.left		= 100;
	bounds.top		= 100;
	bounds.right	= 150;
	bounds.bottom	= 900;
	inWindow.NVI_SetWindowBounds(bounds);
	if( inDocument.SPI_GetWrapMode() == kWrapMode_WordWrap )
	{
		if( inDocument.SPI_GetLineCount() <= linecount )   _ACThrow("",NULL);//ラップ反映されていない！
	}
	else
	{
		if( inDocument.SPI_GetLineCount() != linecount )   _ACThrow("",NULL);
	}
	bounds.left		= 100;
	bounds.top		= 100;
	bounds.right	= 900;
	bounds.bottom	= 900;
	inWindow.NVI_SetWindowBounds(bounds);
	//ドキュメント全削除
	inView.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
	inView.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	
	
	//B0374
	//最初の段落ではB0374発生しないため最初の行は空行とする
	pt.x = 0;
	pt.y = 0;
	text.SetCstring("\r");
	inDocument.SPI_InsertText(pt,text);
	//
	text.DeleteAll();
	for( AIndex i = 0; i < 500; i++ )
	{
		text.AddCstring("あ");
	}
	text.AddCstring("。\r");
	//
	pt.x = 0;
	pt.y = 1;
	inDocument.SPI_InsertText(pt,text);
	//4行目が「あ。」になるまで3行目の最初の文字を削除
	text.SetCstring("あ。");
	AText	deltool("<<<DELETE-NEXT");
	pt.x = 0;
	pt.y = 3;
	while( inDocument.SPI_GetLineLengthWithoutLineEnd(3) > text.GetItemCount() )
	{
		inView.SPI_SetSelect(pt,pt);
		inView.SPI_DoTool_Text(deltool,true,1);
	}
	//再度、4行目の最初の文字を削除
	inView.NVI_ClearDrawTextCount();
	inView.SPI_SetSelect(pt,pt);
	inView.SPI_DoTool_Text(deltool,true,1);
	//4行目は"あ。"になる
	if( inDocument.SPI_GetLineLengthWithoutLineEnd(3) != text.GetItemCount() )   _ACThrow("",NULL);
	//3行分を再描画すべき
	//fprintf(stderr,"%d ",inView.NVMC_GetDrawTextCount());
	if( inView.NVI_GetDrawTextCount() != 2 )   _ACThrow("",NULL);
	//再度、2行目の最初の文字を削除
	inView.NVI_ClearDrawTextCount();
	inView.SPI_SetSelect(pt,pt);
	inView.SPI_DoTool_Text(deltool,true,1);
	//1行目は改行コード有りになる
	if( inDocument.SPI_GetLineExistLineEnd(0) == false )   _ACThrow("",NULL);
	//2行目の行頭に「あ」挿入
	pt.x = 0;
	pt.y = 1;
	text.SetCstring("あ");
	inDocument.SPI_InsertText(pt,text);
	//4行目は"あ。"になる
	text.SetCstring("あ。");
	if( inDocument.SPI_GetLineLengthWithoutLineEnd(3) != text.GetItemCount() )   _ACThrow("",NULL);
	
	//B0349
	//4行目の最初にclass="none"を挿入
	text.SetCstring("class=\"none\" ");
	pt.x = 0;
	pt.y = 3;
	inDocument.SPI_InsertText(pt,text);
	//4行目の最初が'c'でなくなるまで3行目を削除
	while(true)
	{
		pt.x = 0;
		pt.y = 2;
		inView.SPI_SetSelect(pt,pt);
		inView.SPI_DoTool_Text(deltool,true,1);
		pt.x = 0;
		pt.y = 3;
		textpos = inDocument.SPI_GetTextIndexFromTextPoint(pt);
		ch = inDocument.SPI_GetTextChar(textpos);
		if( ch != 'c' )   break;
	}
	//3行目の行頭に「あ」挿入
	pt.x = 0;
	pt.y = 2;
	text.SetCstring("あ");
	inDocument.SPI_InsertText(pt,text);
	//noneを選択
	pt.x = 7;
	pt.y = 3;
	ept.x = 11;
	ept.y = 3;
	inView.SPI_SetSelect(pt,ept);
	//
	text.SetCstring("あ");
	AArray<AIndex>	hilitestyle, hilitespos, hilitepos;
	ABool	updatemenu;
	inView.EVT_DoInlineInput(text,text.GetItemCount(),hilitestyle,hilitespos,hilitepos,0,updatemenu);
	//class="あ"を検索（3行目にあるはず）
	pt.x = 0;
	pt.y = 0;
	inView.SPI_SetSelect(pt,pt);
	AText	findtool("<<<FIND-REGEXP-FROMFIRST\rclass=\"あ\"");
	inView.SPI_DoTool_Text(findtool,true,1);
	inView.SPI_GetSelect(spt,ept);
	if( spt.y != 2 )   _ACThrow("",NULL);
	
	
	
	
	//あ200個
	text.DeleteAll();
	for( AIndex i = 0; i < 200; i++ )
	{
		text.AddCstring("あ");
	}
	WrapTest1(inDocument,inWindow,text);
	//あ2000個<cr>1999個<cr>・・・1980個
	text.DeleteAll();
	for( AIndex n = 2000; n > 1980; n-- )
	{
		for( AIndex i = 0; i < n; i++ )
		{
			text.AddCstring("あ");
		}
		text.Add(kUChar_LineEnd);
	}
	//あ200個<cr>199個<cr>・・・180個
	text.DeleteAll();
	for( AIndex n = 200; n > 180; n-- )
	{
		for( AIndex i = 0; i < n; i++ )
		{
			text.AddCstring("あ");
		}
		text.Add(kUChar_LineEnd);
	}
	WrapTest1(inDocument,inWindow,text);
	//あ20個<cr>19個<cr>・・・0個
	text.DeleteAll();
	for( AIndex n = 20; n > 0; n-- )
	{
		for( AIndex i = 0; i < n; i++ )
		{
			text.AddCstring("あ");
		}
		text.Add(kUChar_LineEnd);
	}
	WrapTest1(inDocument,inWindow,text);
	//ヘルプファイル
	AFileAcc	helpfolder;
	AFileWrapper::GetLocalizedResourceFile("mi Help",helpfolder);
	AObjectArray<AFileAcc>	children;
	helpfolder.GetChildren(children);
	if( children.GetItemCount() == 0 )   _ACThrow("",NULL);
	for( AIndex i = 0; i < children.GetItemCount(); i++ )
	{
		AFileAcc	file = children.GetObject(i);
		file.ReadTo(text);
		AText	filename;
		file.GetFilename(filename);
		AText	suffix;
		filename.GetSuffix(suffix);
		if( suffix.Compare(".png") == true )   continue;
		//if( suffix.Compare(".helpindex") == false )   continue;
		//filename.OutputToStderr();
		WrapTest1(inDocument,inWindow,text);
	}
	
	
	inWindow.NVI_SetWindowBounds(svBounds);
	
	
	return result;
}

ABool	DocumentTest( const ABool inWrapTest )
{
	ABool	result = true;
	ABool	updatemenu;
	
	DocumentTest_ModeSetup();
	
	AFileAcc	testpatfolder;
	AFileWrapper::GetLocalizedResourceFile("TestPatterns",testpatfolder);
	
	AText	text("%%%UnitTest%%%");
	AModeIndex	modeIndex = GetApp().SPI_FindModeIndexByModeRawName(text);
	if( modeIndex == kIndex_Invalid )
	{
		modeIndex = GetApp().SPI_AddNewMode(text);
	}
	
	//新規ドキュメント
	ADocumentID	docID = GetApp().SPNVI_CreateNewTextDocument(modeIndex);
	AWindowID winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
	AControlID	viewID = GetApp().SPI_GetTextWindowByID(winID).NVI_GetCurrentFocus();
	
	ADocument_Text&	doc = GetApp().SPI_GetTextDocumentByID(docID);
	//AWindow_Text&	window = GetApp().SPI_GetTextWindowByID(winID);
	AView_Text&	view = (dynamic_cast<AView_Text&>(GetApp().SPI_GetTextWindowByID(winID).NVI_GetViewByControlID(viewID)));
	
	AFileAcc	toolfolder;
	GetApp().SPI_GetModePrefDB(modeIndex).GetToolFolder(false,toolfolder);
	
	//対応文字列テスト1 #11
	GetApp().SPI_GetModePrefDB(modeIndex).DeleteAll_TextArray(AModePrefDB::kCorrespond_StartText);
	GetApp().SPI_GetModePrefDB(modeIndex).DeleteAll_TextArray(AModePrefDB::kCorrespond_EndText);
	text.SetCstring("<link");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_StartText,text);
	text.SetCstring("");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_EndText,text);
	text.SetCstring("<li");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_StartText,text);
	text.SetCstring("</li>");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_EndText,text);
	text.SetCstring("");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_StartText,text);
	text.SetCstring("test");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_EndText,text);
	text.SetCstring("");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_StartText,text);
	text.SetCstring("");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_EndText,text);
	
	//ドキュメント全削除
	view.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
	view.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	
	//
	text.SetCstring("<link   >");
	view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<link   >") == false )   _ACThrow("",NULL);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<link   >") == false )   _ACThrow("",NULL);
	//
	text.SetCstring("<li>");
	view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<link   ><li></li>") == false )   _ACThrow("",NULL);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<link   ><li></li>") == false )   _ACThrow("",NULL);
	
	//対応文字列テスト2 #12
	GetApp().SPI_GetModePrefDB(modeIndex).DeleteAll_TextArray(AModePrefDB::kCorrespond_StartText);
	GetApp().SPI_GetModePrefDB(modeIndex).DeleteAll_TextArray(AModePrefDB::kCorrespond_EndText);
	text.SetCstring("<link");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_StartText,text);
	text.SetCstring("</link>");//☆
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_EndText,text);
	text.SetCstring("<li");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_StartText,text);
	text.SetCstring("</li>");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_EndText,text);
	text.SetCstring("<title");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_StartText,text);
	text.SetCstring("</title>");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_EndText,text);
	text.SetCstring("if");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_StartText,text);
	text.SetCstring("endif");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_EndText,text);
	text.SetCstring("for");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_StartText,text);
	text.SetCstring("endfor");
	GetApp().SPI_GetModePrefDB(modeIndex).Add_TextArray(AModePrefDB::kCorrespond_EndText,text);
	
	//ドキュメント全削除
	view.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
	view.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	
	//#12
	text.SetCstring("<li>");
	view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<li></li>") == false )   _ACThrow("",NULL);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<li></li>") == false )   _ACThrow("",NULL);
	//
	text.SetCstring("<title>");
	view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<li></li><title></title>") == false )   _ACThrow("",NULL);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<li></li><title></title>") == false )   _ACThrow("",NULL);
	/*
	//☆対応文字列不具合
	//B0443
	text.SetCstring("if");
	view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<li></li><title></title>ifendif") == false )   _ACThrow("",NULL);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<li></li><title></title>ifendif") == false )   _ACThrow("",NULL);
	//
	text.SetCstring("for");
	view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<li></li><title></title>ifendifforendfor") == false )   _ACThrow("",NULL);
	view.EVT_DoMenuItemSelected(kMenuItemID_CorrespondText,GetEmptyText(),0);
	doc.SPI_GetText(text);
	if( text.Compare("<li></li><title></title>ifendifforendfor") == false )   _ACThrow("",NULL);
	*/
	//ドキュメント全削除
	view.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
	view.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	
	//¥を\に変換しないモード
	GetApp().SPI_GetModePrefDB(modeIndex).SetData_Bool(AModePrefDB::kSameAsNormalMode_InputtSettings,false);
	GetApp().SPI_GetModePrefDB(modeIndex).SetData_Bool(AModePrefDB::kInputBackslashByYenKey,false);
	view.EVT_DoMenuItemSelected(kMenuItemID_InputBackslashYen,GetEmptyText(),0);
	AArray<AIndex>	hilitestyle, hilitespos, hilitepos;
	text.SetCstring("¥");
	view.EVT_DoInlineInput(text,text.GetItemCount(),hilitestyle,hilitespos,hilitepos,0,updatemenu);
	view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
	text.SetCstring("\\");
	view.EVT_DoInlineInput(text,text.GetItemCount(),hilitestyle,hilitespos,hilitepos,0,updatemenu);
	view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
	//¥を\に変換するモード
	GetApp().SPI_GetModePrefDB(modeIndex).SetData_Bool(AModePrefDB::kInputBackslashByYenKey,true);
	view.EVT_DoMenuItemSelected(kMenuItemID_InputBackslashYen,GetEmptyText(),0);
	text.SetCstring("¥");
	view.EVT_DoInlineInput(text,text.GetItemCount(),hilitestyle,hilitespos,hilitepos,0,updatemenu);
	view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
	text.SetCstring("\\");
	view.EVT_DoInlineInput(text,text.GetItemCount(),hilitestyle,hilitespos,hilitepos,0,updatemenu);
	view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
	//確認
	doc.SPI_GetText(text);
	if( text.Compare("\\¥¥\\\\¥\\\\\\\\") == false )   _ACThrow("",NULL);
	
	//ドキュメント全削除
	view.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
	view.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	
	//#210
	{
		//文字列挿入
		text.SetCstring("かきくけこ");
		view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("かきくけこ") == false )   _ACThrow("",NULL);
		//Undo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("") == false )   _ACThrow("",NULL);
		//Redo
		view.EVT_DoMenuItemSelected(kMenuItemID_Redo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("かきくけこ") == false )   _ACThrow("",NULL);
		//Undo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("") == false )   _ACThrow("",NULL);
		
		//文字列挿入*2
		text.SetCstring("あい");
		view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
		text.SetCstring("うえお");
		view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("あいうえお") == false )   _ACThrow("",NULL);
		//Undo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("") == false )   _ACThrow("",NULL);
		//Redo
		view.EVT_DoMenuItemSelected(kMenuItemID_Redo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("あいうえお") == false )   _ACThrow("",NULL);
		
		//キャレット設定
		view.SPI_SetSelect(doc.SPI_GetTextLength(),doc.SPI_GetTextLength());
		
		//BS1回
		view.SPI_KeyBindAction(kKeyBindKey_BS,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("あいうえ") == false )   _ACThrow("",NULL);
		//BSをUndo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("あいうえお") == false )   _ACThrow("",NULL);
		//BSをRedo
		view.EVT_DoMenuItemSelected(kMenuItemID_Redo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("あいうえ") == false )   _ACThrow("",NULL);
		
		//キャレット設定
		view.SPI_SetSelect(doc.SPI_GetTextLength(),doc.SPI_GetTextLength());
		
		//BS2回
		view.SPI_KeyBindAction(kKeyBindKey_BS,0,updatemenu);
		view.SPI_KeyBindAction(kKeyBindKey_BS,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("あい") == false )   _ACThrow("",NULL);
		//BS*2をUndo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("あいうえ") == false )   _ACThrow("",NULL);
		//BS*2をRedo
		view.EVT_DoMenuItemSelected(kMenuItemID_Redo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("あい") == false )   _ACThrow("",NULL);
		
		//キャレット設定
		view.SPI_SetSelect(doc.SPI_GetTextLength(),doc.SPI_GetTextLength());
		
		//BS1回
		view.SPI_KeyBindAction(kKeyBindKey_BS,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("あ") == false )   _ACThrow("",NULL);
		//BSをUndo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("あい") == false )   _ACThrow("",NULL);
		//BSをRedo
		view.EVT_DoMenuItemSelected(kMenuItemID_Redo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("あ") == false )   _ACThrow("",NULL);
		//BSをUndo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("あい") == false )   _ACThrow("",NULL);
		//BS*2をUndo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("あいうえ") == false )   _ACThrow("",NULL);
		//BSをUndo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("あいうえお") == false )   _ACThrow("",NULL);
		
		//キャレット設定
		view.SPI_SetSelect(0,0);
		
		//文字列挿入
		text.SetCstring("abc");
		view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("abcあいうえお") == false )   _ACThrow("",NULL);
		//Undo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("あいうえお") == false )   _ACThrow("",NULL);
		//Redo
		view.EVT_DoMenuItemSelected(kMenuItemID_Redo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("abcあいうえお") == false )   _ACThrow("",NULL);
		
		//キャレット設定
		view.SPI_SetSelect(2,2);
		//BS1回
		view.SPI_KeyBindAction(kKeyBindKey_BS,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("acあいうえお") == false )   _ACThrow("",NULL);
		
		//キャレット設定
		view.SPI_SetSelect(doc.SPI_GetTextLength(),doc.SPI_GetTextLength());
		//BS1回
		view.SPI_KeyBindAction(kKeyBindKey_BS,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("acあいうえ") == false )   _ACThrow("",NULL);
		
		//Undo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("acあいうえお") == false )   _ACThrow("",NULL);
		//Undo
		view.EVT_DoMenuItemSelected(kMenuItemID_Undo,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("abcあいうえお") == false )   _ACThrow("",NULL);
		
		//ドキュメント全削除
		view.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
		view.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	}
	
	//#231
	//Drag&Dropを自動テストで実行するのは工数かかるので、マルチバイト文字の途中にキャレット等設定した場合の
	//救済処理のみテストすることにする。
	{
		//文字列挿入
		text.SetCstring("かきくけこ");
		view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("かきくけこ") == false )   _ACThrow("",NULL);
		
		//中途半端な位置に選択範囲を設定
		ATextPoint	spt, ept;
		spt.x = 1;
		spt.y = 0;
		ept.x = 4;
		ept.y = 0;
		view.SPI_SetSelect(spt,ept);
		text.SetCstring("a");
		view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("aきくけこ") == false )   _ACThrow("",NULL);
		
		//中途半端な位置にキャレットを設定
		spt.x = 3;
		spt.y = 0;
		ept.x = 3;
		ept.y = 0;
		view.SPI_SetSelect(spt,ept);
		text.SetCstring("b");
		view.EVT_DoTextInput(text,kKeyBindKey_Invalid,0,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("abきくけこ") == false )   _ACThrow("",NULL);
		
		//ドキュメント全削除
		view.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
		view.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
		
	}
	
	//インデント関連不具合対応
	//B0416（右シフト）
	//複数行を選択中に「右シフト」を行った場合、選択範囲中最初の行の行頭タブ文字が、選択範囲から外れてしまう
	{
		AText	text("012\rabc\rdef\rg");
		ATextPoint	pt = {0,0};
		doc.SPI_InsertText(pt,text);
		//行頭の選択開始位置がずれないことの確認
		ATextPoint	spt = {0,1}, ept = {3,2};
		view.SPI_SetSelect(spt,ept);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftRight,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 4 || ept.y != 2 )   _ACThrow("",NULL);
		//(0,1)->(4,2)
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 3 || ept.y != 2 )   _ACThrow("",NULL);
		//行頭の選択開始位置がずれないことの確認（終了位置は最終行）
		//#581の対策も確認（#581 終了位置が最終位置で左シフトするとインデックスが最終位置を超える）
		ept.x = 1;
		ept.y = 3;
		view.SPI_SetSelect(spt,ept);//(0,1)->(1,3)
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftRight,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 2 || ept.y != 3 )   _ACThrow("",NULL);
		//(0,1)->(2,3)
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 1 || ept.y != 3 )   _ACThrow("",NULL);
		//行頭ではない開始位置がずれることの確認
		spt.x = 1;
		spt.y = 0;
		view.SPI_SetSelect(spt,ept);//(1,0)->(1,3)
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftRight,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 2 || spt.y != 0 || ept.x != 2 || ept.y != 3 )   _ACThrow("",NULL);
		//(2,0)->(2,3)
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 1 || spt.y != 0 || ept.x != 1 || ept.y != 3 )   _ACThrow("",NULL);
		//行全体選択
		//(0,1)->(0,3)
		spt.x = 0;
		spt.y = 1;
		ept.x = 0;
		ept.y = 3;
		view.SPI_SetSelect(spt,ept);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftRight,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 0 || ept.y != 3 )   _ACThrow("",NULL);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 0 || ept.y != 3 )   _ACThrow("",NULL);
		//行内選択（行頭含む）
		spt.x = 0;
		spt.y = 1;
		ept.x = 2;
		ept.y = 1;
		view.SPI_SetSelect(spt,ept);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftRight,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 3 || ept.y != 1 )   _ACThrow("",NULL);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 2 || ept.y != 1 )   _ACThrow("",NULL);
		//行内選択（行頭含まず）
		spt.x = 1;
		spt.y = 1;
		ept.x = 2;
		ept.y = 1;
		view.SPI_SetSelect(spt,ept);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftRight,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 2 || spt.y != 1 || ept.x != 3 || ept.y != 1 )   _ACThrow("",NULL);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 1 || spt.y != 1 || ept.x != 2 || ept.y != 1 )   _ACThrow("",NULL);
		
		//ドキュメント全削除
		view.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
		view.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	}
	
#if 0
	//B0416（インデント）
	{
		/*
<p>(0)
test(1)
</p>(2)
g(3)
		*/
		AText	text("<p>\r\ttest\r</p>\rg");
		ATextPoint	pt = {0,0};
		doc.SPI_InsertText(pt,text);
		//行頭の選択開始位置がずれないことの確認
		ATextPoint	spt = {0,1}, ept = {3,2};
		view.SPI_SetSelect(spt,ept);
		view.EVT_DoMenuItemSelected(kMenuItemID_Indent,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 3 || ept.y != 2 )   _ACThrow("",NULL);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\ttest\r</p>\rg") == false )    _ACThrow("",NULL);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 3 || ept.y != 2 )   _ACThrow("",NULL);
		//行頭の選択開始位置がずれないことの確認（終了位置は最終行）
		//#581の対策も確認
		ept.x = 1;
		ept.y = 3;
		view.SPI_SetSelect(spt,ept);//(0,1)->(1,3)
		view.EVT_DoMenuItemSelected(kMenuItemID_Indent,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 1 || ept.y != 3 )   _ACThrow("",NULL);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\ttest\r</p>\rg") == false )    _ACThrow("",NULL);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 1 || ept.y != 3 )   _ACThrow("",NULL);
		//行頭ではない開始位置がずれることの確認
		spt.x = 1;
		spt.y = 1;
		view.SPI_SetSelect(spt,ept);//(1,1)->(1,3)
		view.EVT_DoMenuItemSelected(kMenuItemID_Indent,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 2 || spt.y != 1 || ept.x != 1 || ept.y != 3 )   _ACThrow("",NULL);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\ttest\r</p>\rg") == false )    _ACThrow("",NULL);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 1 || spt.y != 1 || ept.x != 1 || ept.y != 3 )   _ACThrow("",NULL);
		//行全体選択
		//(0,1)->(0,3)
		spt.x = 0;
		spt.y = 1;
		ept.x = 0;
		ept.y = 3;
		view.SPI_SetSelect(spt,ept);
		view.EVT_DoMenuItemSelected(kMenuItemID_Indent,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 0 || ept.y != 3 )   _ACThrow("",NULL);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\ttest\r</p>\rg") == false )    _ACThrow("",NULL);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 0 || ept.y != 3 )   _ACThrow("",NULL);
		//行内選択（行頭含む）
		spt.x = 0;
		spt.y = 1;
		ept.x = 2;
		ept.y = 1;
		view.SPI_SetSelect(spt,ept);
		view.EVT_DoMenuItemSelected(kMenuItemID_Indent,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 3 || ept.y != 1 )   _ACThrow("",NULL);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\ttest\r</p>\rg") == false )    _ACThrow("",NULL);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 0 || spt.y != 1 || ept.x != 2 || ept.y != 1 )   _ACThrow("",NULL);
		//行内選択（行頭含まず）
		spt.x = 1;
		spt.y = 1;
		ept.x = 2;
		ept.y = 1;
		view.SPI_SetSelect(spt,ept);
		view.EVT_DoMenuItemSelected(kMenuItemID_Indent,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 2 || spt.y != 1 || ept.x != 3 || ept.y != 1 )   _ACThrow("",NULL);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\ttest\r</p>\rg") == false )    _ACThrow("",NULL);
		view.EVT_DoMenuItemSelected(kMenuItemID_ShiftLeft,GetEmptyText(),0);
		view.SPI_GetSelect(spt,ept);
		if( spt.x != 1 || spt.y != 1 || ept.x != 2 || ept.y != 1 )   _ACThrow("",NULL);
		
		//ドキュメント全削除
		view.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
		view.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	}
	
	//正規表現インデント
	//#441 手動インデントの保持（現在行が正規表現一致、または、空行の場合のみインデント適用）
	//#650 選択してインデント実行してもインデントされない←前行が正規表現一致していてもインデント適用されていなかったため
	//（現在行が正規表現一致、または、現在行が空行の場合のみインデント適用されていた。）
	//対策後：改行の現在行・オートインデント文字によるインデントの場合は、現在行のインデント増減＝０の場合、現状インデント維持。それ以外はインデント実行
	{
		AText	text("<p>\r\t\t\t\r\r\t\ttest\r\t\t</p>\r");
		/*
<p>(0)
			(1)
(2)
		test(3)
		</p>(4)
*/
		ABool	updatemenu = false;
		ATextPoint	pt = {0,0};
		doc.SPI_InsertText(pt,text);
		//改行時、マニュアルインデント維持の確認(#441)
		pt.x = 3;
		pt.y = 1;
		view.SPI_SetSelect(pt,pt);
		view.SPI_KeyBindAction(kKeyBindKey_Return,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\t\t\t\r\t\t\t\r\r\t\ttest\r\t\t</p>\r") == false )    _ACThrow("",NULL);
		//改行時、マニュアルインデント維持の確認(#441)
		pt.x = 3;
		pt.y = 2;
		view.SPI_SetSelect(pt,pt);
		view.SPI_KeyBindAction(kKeyBindKey_Return,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\t\t\t\r\t\t\t\r\t\t\t\r\r\t\ttest\r\t\t</p>\r") == false )    _ACThrow("",NULL);
		//改行時、正規表現一致すればインデント適用されることの確認
		pt.x = 6;
		pt.y = 6;
		view.SPI_SetSelect(pt,pt);
		view.SPI_KeyBindAction(kKeyBindKey_Return,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\t\t\t\r\t\t\t\r\t\t\t\r\r\t\ttest\r\t</p>\r\t\r") == false )    _ACThrow("",NULL);
		//再度</p>の後で改行しても、インデント変化しないことの確認
		pt.x = 5;
		pt.y = 6;
		view.SPI_SetSelect(pt,pt);
		view.SPI_KeyBindAction(kKeyBindKey_Return,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\t\t\t\r\t\t\t\r\t\t\t\r\r\t\ttest\r\t</p>\r\t\r\t\r") == false )    _ACThrow("",NULL);
		//範囲選択インデント
		ATextPoint	spt = {0,0}, ept = {0,9};
		view.SPI_SetSelect(spt,ept);
		view.EVT_DoMenuItemSelected(kMenuItemID_Indent,GetEmptyText(),0);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\t\r\t\r\t\r\t\r\ttest\r</p>\r\r\r") == false )    _ACThrow("",NULL);
		//文の途中でのインデント
		pt.x = 2;
		pt.y = 5;
		view.SPI_SetSelect(pt,pt);
		view.SPI_KeyBindAction(kKeyBindKey_Return,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\t\r\t\r\t\r\t\r\tt\r\test\r</p>\r\r\r") == false )    _ACThrow("",NULL);
		
		//ドキュメント全削除
		view.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
		view.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	}
	//#650 <p>＋改行入力してもマニュアルインデント維持されることの確認
	{
		AText	text("<p>\r\t\t\t<p>\r\r\t\ttest\r\t\t</p>\r");
		/*
<p>(0)
			<p>(1)
(2)
		test(3)
		</p>(4)
*/
		ABool	updatemenu = false;
		ATextPoint	pt = {0,0};
		doc.SPI_InsertText(pt,text);
		//改行時、マニュアルインデント維持の確認
		pt.x = 6;
		pt.y = 1;
		view.SPI_SetSelect(pt,pt);
		view.SPI_KeyBindAction(kKeyBindKey_Return,0,updatemenu);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\t\t\t<p>\r\t\t\t\t\r\r\t\ttest\r\t\t</p>\r") == false )    _ACThrow("",NULL);
		
		//ドキュメント全削除
		view.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
		view.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	}
	
	//ツールコマンド
	{
		//
		AText	text;
		AText	tooltext;
		ATextPoint	spt,ept;
		spt.x = 0;
		spt.y = 0;
		text.SetCstring("abc");
		doc.SPI_InsertText(spt,text);
		ept.x = 3;
		ept.y = 0;
		view.SPI_SetSelect(spt,ept);
		tooltext.SetCstring("<<<TOOLINDENT>>><p>\r<<<SELECTED CARET>>>\r</p>");
		view.SPI_DoTool_Text(tooltext,true,1);
		doc.SPI_GetText(text);
		if( text.Compare("<p>\r\tabc\r</p>") == false )   _ACThrow("",NULL);
		
		//ドキュメント全削除
		view.EVT_DoMenuItemSelected(kMenuItemID_SelectAll,GetEmptyText(),0);
		view.EVT_DoMenuItemSelected(kMenuItemID_Clear,GetEmptyText(),0);
	}
#endif
	
	//AppleScriptツール
	AFileAcc	file;
	file.SpecifyChild(toolfolder,"testscript1.scpt");
	AText	path;
	file.GetPath(path);
	view.EVT_DoMenuItemSelected(kMenuItemID_Tool,path,0);
	doc.SPI_GetText(text);
	if( text.Compare("TEST") == false )   _ACThrow("",NULL);
	//B0405 TOOLからAppleScript
	file.SpecifyChild(toolfolder,"Tool2");
	file.GetPath(path);
	view.EVT_DoMenuItemSelected(kMenuItemID_Tool,path,0);
	doc.SPI_GetText(text);
	if( text.Compare("TESTTEST") == false )   _ACThrow("",NULL);
	
	//行折り返しテスト
	
	if( inWrapTest == true )
	{
		//ワードラップモードに設定
		doc.SPI_SetWrapMode(kWrapMode_WordWrap,80);
		if( WrapTest(doc,GetApp().SPI_GetTextWindowByID(winID),view) == false )   _ACThrow("",NULL);
		
		//文字数ラップモードに設定
		doc.SPI_SetWrapMode(kWrapMode_WordWrapByLetterCount,80);
		if( WrapTest(doc,GetApp().SPI_GetTextWindowByID(winID),view) == false )   _ACThrow("",NULL);
	}
	
	//#379 doc.SPI_ExecuteClose();
	GetApp().SPI_ExecuteCloseTextDocument(doc.GetObjectID());
	return result;
}

ABool	RecentOpenTest()
{
	ABool	result = true;
	
	ATextArray	svCommentArray, svCommentPathArray;
	svCommentArray.SetFromTextArray(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFileComment));
	svCommentPathArray.SetFromTextArray(GetApp().GetAppPref().GetData_ConstTextArrayRef(AAppPrefDB::kRecentlyOpenedFileCommentPath));
	ABool	svDontAdd = GetApp().GetAppPref().GetData_Bool(AAppPrefDB::kDontRememberAnyHistory);
	GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kDontRememberAnyHistory,false);
	
	GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kRecentlyOpenedFileComment);
	GetApp().GetAppPref().DeleteAll_TextArray(AAppPrefDB::kRecentlyOpenedFileCommentPath);
	
	try
	{
		AFileAcc	testfolder;
		testfolder.SpecifyAsUnitTestFolder();
		
		AFileAcc	folder1, folder2;
		folder1.SpecifyChild(testfolder,"folder1");
		folder1.CreateFolder();
		folder2.SpecifyChild(testfolder,"folder2");
		folder2.CreateFolder();
		
		AFileAcc	file1, file2;
		file1.SpecifyChild(folder1,"file.txt");
		file1.CreateFile();
		file2.SpecifyChild(folder2,"file.txt");
		file2.CreateFile();
		
		AObjectID	doc1ID = GetApp().SPNVI_CreateDocumentFromLocalFile(file1);
		AObjectID	doc2ID = GetApp().SPNVI_CreateDocumentFromLocalFile(file2);
		
		AText	text;
		AText	t;
		AText	p;
		
		//データ確認
		t.SetCstring("file.txt  (");
        //folder1.GetPath(p);
        GetApp().GetAppPref().GetFilePathForDisplay(folder1,p);
		t.AddText(p);
		t.AddCstring(")");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,1,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		t.SetCstring("file.txt  (");
        //folder2.GetPath(p);
        GetApp().GetAppPref().GetFilePathForDisplay(folder2,p);
		t.AddText(p);
		t.AddCstring(")");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,0,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		//更新（全て）
		GetApp().SPI_UpdateRecentlyOpenedFile(false);
		
		//データ確認
		t.SetCstring("file.txt  (");
        //folder1.GetPath(p);
        GetApp().GetAppPref().GetFilePathForDisplay(folder1,p);
		t.AddText(p);
		t.AddCstring(")");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,1,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		t.SetCstring("file.txt  (");
        //folder2.GetPath(p);
        GetApp().GetAppPref().GetFilePathForDisplay(folder2,p);
		t.AddText(p);
		t.AddCstring(")");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,0,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		//更新（最初のみ）
		GetApp().SPI_UpdateRecentlyOpenedFile(true);
		
		//データ確認
		t.SetCstring("file.txt  (");
        //folder1.GetPath(p);
        GetApp().GetAppPref().GetFilePathForDisplay(folder1,p);
		t.AddText(p);
		t.AddCstring(")");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,1,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		t.SetCstring("file.txt  (");
        //folder2.GetPath(p);
        GetApp().GetAppPref().GetFilePathForDisplay(folder2,p);
		t.AddText(p);
		t.AddCstring(")");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,0,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		GetApp().SPI_TryCloseTextDocument(doc1ID);//#379 SPI_GetTextDocumentByID(doc1ID).SPI_TryClose();
		GetApp().SPI_TryCloseTextDocument(doc2ID);//#379 SPI_GetTextDocumentByID(doc2ID).SPI_TryClose();
		
		
		//順番を変えてopen
		doc2ID = GetApp().SPNVI_CreateDocumentFromLocalFile(file2);
		doc1ID = GetApp().SPNVI_CreateDocumentFromLocalFile(file1);
		
		//データ確認
		t.SetCstring("file.txt  (");
        //folder1.GetPath(p);
        GetApp().GetAppPref().GetFilePathForDisplay(folder1,p);
		t.AddText(p);
		t.AddCstring(")");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,0,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		t.SetCstring("file.txt  (");
        //folder2.GetPath(p);
        GetApp().GetAppPref().GetFilePathForDisplay(folder2,p);
		t.AddText(p);
		t.AddCstring(")");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,1,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		GetApp().SPI_TryCloseTextDocument(doc1ID);//#379 SPI_GetTextDocumentByID(doc1ID).SPI_TryClose();
		GetApp().SPI_TryCloseTextDocument(doc2ID);//#379 SPI_GetTextDocumentByID(doc2ID).SPI_TryClose();
		
		//注釈設定1
        //folder1.GetPath(p);
        GetApp().GetAppPref().GetFilePathForDisplay(folder1,p);
		GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kRecentlyOpenedFileCommentPath,p);
		text.SetCstring("森鷗外");
		GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kRecentlyOpenedFileComment,text);
		GetApp().SPI_UpdateRecentlyOpenedFile(false);
		
		//open
		doc2ID = GetApp().SPNVI_CreateDocumentFromLocalFile(file2);
		doc1ID = GetApp().SPNVI_CreateDocumentFromLocalFile(file1);
		
		//データ確認
		t.SetCstring("file.txt  森鷗外");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,0,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		t.SetCstring("file.txt");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,1,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		GetApp().SPI_TryCloseTextDocument(doc1ID);//#379 SPI_GetTextDocumentByID(doc1ID).SPI_TryClose();
		GetApp().SPI_TryCloseTextDocument(doc2ID);//#379 SPI_GetTextDocumentByID(doc2ID).SPI_TryClose();
		
		//注釈設定2
		AFileAcc	parent;
		parent.SpecifyParent(folder2);
		parent.GetPath(p);
		GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kRecentlyOpenedFileCommentPath,p);
		text.SetCstring("==TEST==");
		GetApp().GetAppPref().Add_TextArray(AAppPrefDB::kRecentlyOpenedFileComment,text);
		GetApp().SPI_UpdateRecentlyOpenedFile(false);
		
		//open
		doc2ID = GetApp().SPNVI_CreateDocumentFromLocalFile(file2);
		doc1ID = GetApp().SPNVI_CreateDocumentFromLocalFile(file1);
		
		//データ確認
		t.SetCstring("file.txt  森鷗外");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,0,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		t.SetCstring("file.txt  ==TEST==");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,1,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		GetApp().SPI_TryCloseTextDocument(doc1ID);//#379 SPI_GetTextDocumentByID(doc1ID).SPI_TryClose();
		GetApp().SPI_TryCloseTextDocument(doc2ID);//#379 SPI_GetTextDocumentByID(doc2ID).SPI_TryClose();
		
		//注釈設定3
		text.SetCstring("森鷗外");
		GetApp().GetAppPref().SetData_TextArray(AAppPrefDB::kRecentlyOpenedFileComment,1,text);
		GetApp().SPI_UpdateRecentlyOpenedFile(false);
		
		//open
		doc2ID = GetApp().SPNVI_CreateDocumentFromLocalFile(file2);
		doc1ID = GetApp().SPNVI_CreateDocumentFromLocalFile(file1);
		
		//データ確認
		t.SetCstring("file.txt  森鷗外  (");
        //folder1.GetPath(p);
        GetApp().GetAppPref().GetFilePathForDisplay(folder1,p);
		t.AddText(p);
		t.AddCstring(")");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,0,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		t.SetCstring("file.txt  森鷗外  (");
        //folder2.GetPath(p);
        GetApp().GetAppPref().GetFilePathForDisplay(folder2,p);
		t.AddText(p);
		t.AddCstring(")");
		GetApp().NVI_GetMenuManager().GetDynamicMenuItemMenuTextByMenuItemID(kMenuItemID_Open_RecentlyOpenedFile,1,text);
		if( text.Compare(t) == false )   _ACThrow("",NULL);
		
		GetApp().SPI_TryCloseTextDocument(doc1ID);//#379 SPI_GetTextDocumentByID(doc1ID).SPI_TryClose();
		GetApp().SPI_TryCloseTextDocument(doc2ID);//#379 SPI_GetTextDocumentByID(doc2ID).SPI_TryClose();
		
	}
	catch(...)
	{
		result = false;
	}
	
	GetApp().GetAppPref().SetData_TextArray(AAppPrefDB::kRecentlyOpenedFileComment,svCommentArray);
	GetApp().GetAppPref().SetData_TextArray(AAppPrefDB::kRecentlyOpenedFileCommentPath,svCommentPathArray);
	GetApp().GetAppPref().SetData_Bool(AAppPrefDB::kDontRememberAnyHistory,svDontAdd);
	
	return result;
}

ABool	AbsTest( const ABool inWrapTest )
{
	ABool	result = true;
	
	AFileAcc	testfolder;
	testfolder.SpecifyAsUnitTestFolder();
	
	try
	{
		{
			if( TextInfoTest() == false )   result = false;
		}
		fprintf(stderr,"TextInfo test done\n");
		//#598
		{
		//	if( ThreadTest() == false )   result = false;
		}
		fprintf(stderr,"ThreadTest done\n");
		
		/*
		//☆モード設定テスト未対応
		{
			if( ModePrefWindowTest() == false )   result = false;
		}
		fprintf(stderr,"ModePrefWindowTest done\n");
		*/
		{
			if( RecentOpenTest() == false )
			{
				result = false;
				_ACThrow("",NULL);
			}
		}
		fprintf(stderr,"RecentOpenTest done\n");
		{
			if( TransliterateTest() == false )   result = false;
		}
		fprintf(stderr,"TransliterateTest done\n");
		{
			if( DocumentTest(inWrapTest) == false )   result = false;
		}
		fprintf(stderr,"DocumentTest done\n");
		//#567
		{
			if( LuaTest() == false )   result = false;
		}
		fprintf(stderr,"Lua Test done\n");
		/* #1006により、このルートだとInternalEvent処理できないので、ファイルを開く箇所でAppleScriptエラーとなってしまうので、とりあえずここは外す
		{
			GetApp().SPI_GetTraceLogWindow().NVI_Hide();
			if( AppleScriptTest() == false )
			{
				result = false;
				_ACThrow("",NULL);
			}
			GetApp().SPI_GetTraceLogWindow().NVI_Show();
		}
		fprintf(stderr,"AppleScriptTest done\n");
		*/
		AText	utf8("UTF-8");
		AText	utf16("UTF-16");
		AText	utf16LE("UTF-16LE");
		AText	utf16BE("UTF-16BE");
		//
		_AInfo("Unicode Document Test",NULL);
		{
			AText	text;
			MakeAllUnicodeCharText(text);//UTF-8で取得
			text.Delete(10,1);//LFはCRに変換されてしまうので消しておく
			{
				//UTF-8で元ファイル保存（このファイルはテストには使用していない）
				AFileAcc	file0;
				file0.SpecifyChild(testfolder,"UnicodeDocumentTest0.txt");
				file0.CreateFile();
				file0.WriteFile(text);//UTF-8保存
			}
			AFileAcc	file;
			file.SpecifyChild(testfolder,"UnicodeDocumentTest.txt");
			file.CreateFile();
			file.WriteFile(text);//UTF-8保存
			//UTF-8ファイルを開いてUTF-16で保存（"UnicodeDocumentTest.txt"へ保存）
			{
				AObjectID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,utf8);
				//#695 書き込み不可状態を解除するため。あくまでテストのための処理。
				GetApp().SPI_GetTextDocumentByID(docID).SPI_TransitInitState_WrapCalculateCompleted();
				//
				GetApp().SPI_GetTextDocumentByID(docID).SPI_SetTextEncoding(utf16);
				GetApp().SPI_GetTextDocumentByID(docID).SPI_Save(false);//UTF-16保存
				AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
				GetApp().SPI_GetTextWindowByID(winID).SPI_TryCloseDocument(docID);
			}
			//UTF-16ファイル（"UnicodeDocumentTest.txt"）を開いて、内部テキストを比較
			{
				AObjectID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,utf16);
				
				//比較
				AText	t;
				GetApp().SPI_GetTextDocumentByID(docID).SPI_GetText(t);
				{
					AFileAcc	file2;
					file2.SpecifyChild(testfolder,"UnicodeDocumentTest2.txt");
					file2.CreateFile();
					t.ConvertFromUTF8ToUTF16();
					file2.WriteFile(t);//UTF-16保存（このファイルはテストには使用していない）
					t.ConvertToUTF8FromUTF16();
				}
				if( t.Compare(text) == false )   _ACThrow("",NULL);
				
				AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
				GetApp().SPI_GetTextWindowByID(winID).SPI_TryCloseDocument(docID);
			}
		}
		fprintf(stderr,"Unicode Document Test done\n");
		//B0400
		{
			//UTF-16 BOM無しファイル(=BE)作成→ドキュメント読み込み→比較
			AText	text;
			text.Add(0);
			text.Add('A');
			AFileAcc	file;
			file.SpecifyChild(testfolder,"UDT_2_UTF16NoBOM");
			file.CreateFile();
			file.WriteFile(text);
			AObjectID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,utf16);
			AText	t;
			GetApp().SPI_GetTextDocumentByID(docID).SPI_GetText(t);
			if( t.Compare("A") == false )   _ACThrow("",NULL);
			GetApp().SPI_GetTextDocumentByID(docID).SPI_Save(false);
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			GetApp().SPI_GetTextWindowByID(winID).SPI_TryCloseDocument(docID);
			
			GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_UnitTest,file,t);
			if( t.Compare("A") == false )   _ACThrow("",NULL);
		}
		{
			//UTF-16 BE BOM(FE FF)ありファイル作成→ドキュメント読み込み→比較
			AText	text;
			text.Add(0xFE);
			text.Add(0xFF);
			text.Add(0);
			text.Add('A');
			AFileAcc	file;
			file.SpecifyChild(testfolder,"UDT_2_UTF16BOM_BE");
			file.CreateFile();
			file.WriteFile(text);
			AObjectID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,utf16);
			AText	t;
			GetApp().SPI_GetTextDocumentByID(docID).SPI_GetText(t);
			if( t.Compare("A") == false )   _ACThrow("",NULL);
			GetApp().SPI_GetTextDocumentByID(docID).SPI_Save(false);
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			GetApp().SPI_GetTextWindowByID(winID).SPI_TryCloseDocument(docID);
			
			GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_UnitTest,file,t);
			if( t.Compare("A") == false )   _ACThrow("",NULL);
		}
		{
			//UTF-16 LE BOM(FF FE)ありファイル作成→ドキュメント読み込み→比較
			AText	text;
			text.Add(0xFF);
			text.Add(0xFE);
			text.Add('A');
			text.Add(0);
			AFileAcc	file;
			file.SpecifyChild(testfolder,"UDT_2_UTF16BOM_LE");
			file.CreateFile();
			file.WriteFile(text);
			AObjectID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,utf16);
			AText	t;
			GetApp().SPI_GetTextDocumentByID(docID).SPI_GetText(t);
			if( t.Compare("A") == false )   _ACThrow("",NULL);
			GetApp().SPI_GetTextDocumentByID(docID).SPI_Save(false);
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			GetApp().SPI_GetTextWindowByID(winID).SPI_TryCloseDocument(docID);
			
			GetApp().SPI_LoadTextFromFile(kLoadTextPurposeType_UnitTest,file,t);
			if( t.Compare("A") == false )   _ACThrow("",NULL);
		}
		{
			//UTF-16BE BOM無しファイル作成→ドキュメント読み込み→比較
			AText	text;
			text.Add(0);
			text.Add('A');
			AFileAcc	file;
			file.SpecifyChild(testfolder,"UDT_2_UTF16BENoBOM");
			file.CreateFile();
			file.WriteFile(text);
			AObjectID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,utf16BE);
			AText	t;
			GetApp().SPI_GetTextDocumentByID(docID).SPI_GetText(t);
			if( t.Compare("A") == false )   _ACThrow("",NULL);
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			GetApp().SPI_GetTextWindowByID(winID).SPI_TryCloseDocument(docID);
		}
		{
			//UTF-16LE BOM無しファイル作成→ドキュメント読み込み→比較
			AText	text;
			text.Add('A');
			text.Add(0);
			AFileAcc	file;
			file.SpecifyChild(testfolder,"UDT_2_UTF16LENoBOM");
			file.CreateFile();
			file.WriteFile(text);
			AObjectID	docID = GetApp().SPNVI_CreateDocumentFromLocalFile(file,utf16LE);
			AText	t;
			GetApp().SPI_GetTextDocumentByID(docID).SPI_GetText(t);
			if( t.Compare("A") == false )   _ACThrow("",NULL);
			AWindowID	winID = GetApp().SPI_GetTextDocumentByID(docID).SPI_GetFirstWindowID();
			GetApp().SPI_GetTextWindowByID(winID).SPI_TryCloseDocument(docID);
		}
		fprintf(stderr,"AbsTest done\n");
	}
	catch(...)
	{
		result = false;
	}
	return result;
}

