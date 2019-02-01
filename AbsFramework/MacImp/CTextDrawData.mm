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

CTextDrawData

*/

#include "CTextDrawData.h"
#include "../Frame.h"//#414

#pragma mark ===========================
#pragma mark [クラス]CTextDrawData
#pragma mark ===========================
//TextDrawDataクラス
//色等の情報付き描画用データを格納し、Impクラスに渡すためのクラス

#pragma mark ---コンストラクタ／デストラクタ

/**
コンストラクタ
*/
CTextDrawData::CTextDrawData( const ABool inDisplayEachCanonicalDecompChar ) : /*#1034 mTextLayout(NULL), mATSUStyle(NULL),*/ 
		mCTLineRef(NULL),mCTTypesetterRef(NULL)//#1034
,mFontFallbackEnabled(true), /*#1034 mReuseATSUStyle(false),*/ mDisplayEachCanonicalDecompChar(inDisplayEachCanonicalDecompChar)
,selectionFrameAlpha(0.0), selectionBackgroundColor(kColor_White)//#1316
{
	//B0386 改行コード位置に変な色が表示されることがある問題修正
	selectionStart = 0;
	//B0000 行折り返し計算高速化
	Init();
}
/**
デストラクタ
*/
CTextDrawData::~CTextDrawData()
{
	OSStatus	err = noErr;
	
	//CTLine解放 #1034
	ClearCTLineRef();
	
	//CTTypesetter解放 #1034
	ClearCTTypesetterRef();
	
	/*#1034
	//高速化のため、一度計算したtextLayoutはオブジェクト削除まで保持する。
	if( mTextLayout != NULL )
	{
		err = ::ATSUDisposeTextLayout(mTextLayout);
		if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,NULL);
		mTextLayout = NULL;
	}
	if( mATSUStyle != NULL )
	{
		err = ::ATSUDisposeStyle(mATSUStyle);
		if( err != noErr )   _ACErrorNum("ATSUDisposeStyle() returned error: ",err,NULL);
		mATSUStyle = NULL;
	}
	*/
}

//B0000 行折り返し計算高速化
/**
全データ初期化
*/
void	CTextDrawData::Init()
{
	OSStatus	err = noErr;
	
	//CTLine解放 #1034
	ClearCTLineRef();
	
	//CTTypesetter解放 #1034
	ClearCTTypesetterRef();
	
	/*#1034
	if( mTextLayout != NULL )
	{
		err = ::ATSUDisposeTextLayout(mTextLayout);
		if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,NULL);
		mTextLayout = NULL;
	}
	if( mATSUStyle != NULL && mReuseATSUStyle == false )//#695
	{
		err = ::ATSUDisposeStyle(mATSUStyle);
		if( err != noErr )   _ACErrorNum("ATSUDisposeStyle() returned error: ",err,NULL);
		mATSUStyle = NULL;
	}
	*/
	
	UTF16DrawText.DeleteAll();
	OriginalTextArray_UnicodeOffset.DeleteAll();
	UTF16DrawTextArray_OriginalTextIndex.DeleteAll();
	OriginalTextArray_TabLetters.DeleteAll();
	attrPos.DeleteAll();
	attrColor.DeleteAll();
	attrStyle.DeleteAll();
	additionalAttrPos.DeleteAll();//#1316
	additionalAttrLength.DeleteAll();//#1316
	additionalAttrColor.DeleteAll();//#1316
	additionalAttrStyle.DeleteAll();//#1316
	drawSelection = false;
	selectionStart = kIndex_Invalid;
	selectionEnd = kIndex_Invalid;
	selectionColor = kColor_Blue;
	selectionAlpha = 0.8;
	selectionFrameAlpha = 0.0;//#1316
	selectionBackgroundColor = kColor_White;//#1316
	misspellStartArray.DeleteAll();
	misspellEndArray.DeleteAll();
	startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex.DeleteAll();//#117
	controlCodeStartUTF8Index.DeleteAll();//#473
	controlCodeEndUTF8Index.DeleteAll();//#473
	hintTextStartUTF8Index.DeleteAll();
	hintTextEndUTF8Index.DeleteAll();
}

#pragma mark ===========================

#pragma mark ---TextLayout設定

#if 0 
/**
TextLayout設定
*/
void	CTextDrawData::SetTextLayout( ATSUTextLayout inTextLayout )
{
	OSStatus	err = noErr;
	
	if( mTextLayout != NULL )
	{
		err = ::ATSUDisposeTextLayout(mTextLayout);
		if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,NULL);
		mTextLayout = NULL;
	}
	mTextLayout = inTextLayout;
}

//#532
/**
TextLayoutキャッシュをクリア
*/
void	CTextDrawData::ClearTextLayout()
{
	OSStatus	err = noErr;
	
	if( mTextLayout != NULL )
	{
		err = ::ATSUDisposeTextLayout(mTextLayout);
		if( err != noErr )   _ACErrorNum("ATSUDisposeTextLayout() returned error: ",err,NULL);
		mTextLayout = NULL;
	}
}
#endif

//#1034
/**
CTLine設定
*/
void	CTextDrawData::SetCTLineRef( CTLineRef inCTLineRef )
{
	ClearCTLineRef();
	mCTLineRef = inCTLineRef;
}

//#1034
/**
CTLine解放
*/
void	CTextDrawData::ClearCTLineRef()
{
	if( mCTLineRef != NULL )
	{
		::CFRelease(mCTLineRef);
		mCTLineRef = NULL;
	}
}

//#1034
/**
CTLine設定
*/
void	CTextDrawData::SetCTTypesetterRef( CTTypesetterRef inCTTypesetterRef )
{
	ClearCTTypesetterRef();
	mCTTypesetterRef = inCTTypesetterRef;
}

//#1034
/**
CTLine解放
*/
void	CTextDrawData::ClearCTTypesetterRef()
{
	if( mCTTypesetterRef != NULL )
	{
		::CFRelease(mCTTypesetterRef);
		mCTTypesetterRef = NULL;
	}
}

#pragma mark ===========================

#pragma mark --- TextDrawData作成

//win AView_Edit作成のため、ATextInfoからコピーして作成
/**
テキスト描画用データ生成

UTF16化、タブのスペース化、制御文字の可視化等を行う。
CTextDrawDataクラスのうち、下記を設定する。
UTF16DrawText: UTF16での描画用テキスト
UnicodeDrawTextIndexArray: インデックス：ドキュメントが保持している元のテキストのインデックス　値：描画テキストのUnicode文字単位でのインデックス
*/
void	CTextDrawData::MakeTextDrawDataWithoutStyle( const AText& inText, const ANumber inTabWidth, const AItemCount inLetterCountLimit, 
			const ABool inGetUTF16Text, const ABool inCountAs2Letters, const ABool inDisplayControlCode,
			const AIndex inStartIndex, const AItemCount inLength,
			const ABool inStartSpaceToIndent, const AItemCount inStartSpaceCount, const AItemCount inStartIndentWidth )//#117
{
	AArray<AIndex>	hintTextPosArray;
	ATextArray	hintTextArray;
	MakeTextDrawDataWithoutStyle(inText,inTabWidth,inLetterCountLimit,inGetUTF16Text,inCountAs2Letters,
								 inDisplayControlCode,inStartIndex,inLength,
								 hintTextPosArray, hintTextArray,
								 inStartSpaceToIndent,inStartSpaceCount,inStartIndentWidth,false
								 );
}
void	CTextDrawData::MakeTextDrawDataWithoutStyle( const AText& inText, 
													const ANumber inTabWidth, 
													const AItemCount inLetterCountLimit, 
													const ABool inGetUTF16Text, 
													const ABool inCountAs2Letters, 
													const ABool inDisplayControlCode,
													const AIndex inStartIndex, 
													const AItemCount inLength,
													const AArray<AIndex>& inHintTextPosArray,
													const ATextArray& inHintTextArray,
													const ABool inStartSpaceToIndent, 
													const AItemCount inStartSpaceCount, 
													const AItemCount inStartIndentWidth,
													const ABool inDisplayYenFor5C
													)//#117
{
	//B0000 行折り返し計算高速化
	Init();
	
	//unicode data取得
	AUnicodeData&	unicodeData = AApplication::GetApplication().NVI_GetUnicodeData();
	
	//行情報取得
	AIndex	lineInfo_Start = inStartIndex;
	//B0000 行折り返し計算高速化
	AItemCount	lineInfo_LengthWithoutCR = inLength;
	
	//#733
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	const AUChar*	textp = textptr.GetPtr();
	const AItemCount	textlen = textptr.GetByteCount();
	
	//inTextを参照して、Unicode文字１文字毎にタブスペース化、制御文字可視化を行って、UTF8DrawTextへ格納する
	//また、同時に元のテキストと、描画用Unicodeテキストとの位置対応関係を配列に格納する。
	//UTF16DrawTextArray_OriginalTextIndex: 元のテキストのインデックス(index)が値に入る配列。添字はUnicode文字の文字数(uniOffset)。マウスクリック位置からテキスト位置への変換に使われる。
	//OriginalTextArray_UnicodeOffset: Unicode文字の文字数(uniOffset)が値に入る配列。添字は元のテキストのインデックス(index)
	AIndex	uniOffset = 0;
	AIndex	tabletters = 0;//B0154
	AIndex	tabinc0800 = 1;//B0154
	if( inCountAs2Letters )   tabinc0800 = 2;//B0154
	AIndex	originalTextIndex = 0;
	AIndex	nextOriginalTextIndex = 0;
	ABool	gyoutou = true;//#117
	AUCS4Char	nextUCS4Char = 0;//#863
	AItemCount	nextbc = 1;//#863
	ABool	nextUCS4CharLoaded = false;//#863
	for( ; originalTextIndex < lineInfo_LengthWithoutCR; originalTextIndex = nextOriginalTextIndex )
	{
		if( inLetterCountLimit > 0 )
		{
			if( tabletters >= inLetterCountLimit )   break;
		}
		
		//==================文字取得==================
		AItemCount	bc = 1;
		AUCS4Char	ucs4Char = 0;//#863
		if( nextUCS4CharLoaded == true )
		{
			//次の文字ロード済みならそれを使用
			ucs4Char = nextUCS4Char;
			bc = nextbc;
		}
		else
		{
			//次の文字未ロードなら、文字読み込み
			bc = AText::Convert1CharToUCS4(textp,textlen,lineInfo_Start+originalTextIndex,ucs4Char);//#733
		}
		//次の文字ロード済みフラグOFF
		nextUCS4CharLoaded = false;
		//次の文字をロード
		if( originalTextIndex+bc < lineInfo_LengthWithoutCR )
		{
			nextbc = AText::Convert1CharToUCS4(textp,textlen,lineInfo_Start+originalTextIndex+bc,nextUCS4Char);
			nextUCS4CharLoaded = true;
			//次の文字がdecomp文字の場合、comp文字に変換する
			/*#1051 OS10.9ではdecompになっていても1文字で表示されるので結合する必要性がなくなったため、常にmDisplayEachCanonicalDecompChar==true（2文字分離）として扱う
			if( mDisplayEachCanonicalDecompChar == false && unicodeData.IsCanonicalDecomp2ndChar(nextUCS4Char) == true )
			{
				//------------------Canonical Comp文字（結合後文字）を取得して、今回処理するUCS4文字(ucs4char)を置き換え------------------
				AUCS4Char	compchar = unicodeData.GetCanonicalCompChar(nextUCS4Char,ucs4Char);
				if( compchar != 0 )
				{
					//今回処理するUCS4文字をComp文字に置き換え
					ucs4Char = compchar;
					//今回の文字長はdecomp文字の後までにする
					bc += nextbc;
					//次の文字ロード済みフラグOFF
					nextUCS4CharLoaded = false;
				}
			}
			*/
		}
		//次の文字の位置設定
		nextOriginalTextIndex = originalTextIndex + bc;
		//スペース・タブ以外だったら行頭フラグをfalseにする。
		if( ucs4Char != kUChar_Space && ucs4Char != kUChar_Tab )   gyoutou = false;//#117
		
		//ヒントテキスト位置であればヒントのindexを取得
		AIndex	hintTextArrayIndex = inHintTextPosArray.Find(originalTextIndex);
		
		//==================改行コードの場合==================
		if( ucs4Char == kUChar_LineEnd && bc == 1 )//イリーガルなUTF-8文字列c0 8d等の場合、Convert1CharToUCS4()で0x0Dと変換してしまうので、それを排除するためbcもチェック。（改行コードは正しく読まないと、行折り返し計算で無限ループする）
		{
			break;
		}
		//==================タブコードの場合==================
		else if( ucs4Char == kUChar_Tab && bc == 1)
		{
			//タブ文字
			
			//------------------オリジナルテキストに対応する配列にデータ設定------------------
			for( AIndex i = 0; i < bc; i++ )
			{
				//UTF16DrawText内の対応するindexを設定（今のunicode offset）
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				//タブ用文字カウント数を設定
				OriginalTextArray_TabLetters.Add(tabletters);
			}
			
			//------------------Unicode一文字（スペース）追加------------------
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
			UTF16DrawText.AddUTF16Char(kUChar_Space);
			uniOffset++;
			
			//
			tabletters++;//B0154
			while( (tabletters%inTabWidth) != 0 )//B0154
			{
				//Unicode一文字追加
				//タブ部分の前半／後半で、対応する元のテキストの位置を変える。
				if( (tabletters%inTabWidth) > inTabWidth/2 )
				{
					UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex+1);
				}
				else
				{
					UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				}
				UTF16DrawText.AddUTF16Char(kUChar_Space);
				uniOffset++;
				
				//
				tabletters++;//B0154
			}
		}
		//==================ヒントテキスト==================
		else if( hintTextArrayIndex != kIndex_Invalid )
		{
			//------------------オリジナルテキストに対応する配列にデータ設定------------------
			for( AIndex i = 0; i < bc; i++ )
			{
				//UTF16DrawText内の対応するindexを設定（今のunicode offset）
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				//タブ用文字カウント数を設定
				OriginalTextArray_TabLetters.Add(tabletters);
			}
			
			//ヒントテキスト追加
			AText	text;
			inHintTextArray.Get(hintTextArrayIndex,text);
			text.InsertCstring(0,"  ");
			text.AddCstring("  ");
			text.ConvertFromUTF8ToUTF16();
			UTF16DrawText.AddText(text);
			//
			for( AIndex i = 0; i < text.GetItemCount()/2; i++ )
			{
				UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				uniOffset++;
			}
			//ヒントテキスト位置記憶
			hintTextStartUTF8Index.Add(originalTextIndex);
			hintTextEndUTF8Index.Add(originalTextIndex+1);
		}
		
		//==================制御文字の場合==================
		else if( inDisplayControlCode == true && ucs4Char <= 0x1F && bc == 1)
		{
			//制御文字をHEX表示する
			
			//UTF-8エラー文字の場合、Convert1CharToUCS4()によりucs4Charは0x00になっているので、
			//このばあいに元のコードを表示するために、再度生データを読み込む。
			AUChar	ch = inText.Get(lineInfo_Start+originalTextIndex);
			
			//------------------オリジナルテキストに対応する配列にデータ設定------------------
			for( AIndex i = 0; i < bc; i++ )
			{
				//UTF16DrawText内の対応するindexを設定（今のunicode offset）
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				//タブ用文字カウント数を設定
				OriginalTextArray_TabLetters.Add(tabletters);
			}
			
			//Unicode一文字追加
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
			UTF16DrawText.AddUTF16Char(kUChar_Space);
			uniOffset++;
			//Unicode一文字追加
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
			UTF16DrawText.AddUTF16Char(hex2asc((ch&0xF0)/0x10));
			uniOffset++;
			//Unicode一文字追加
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
			UTF16DrawText.AddUTF16Char(hex2asc(ch&0xF));
			uniOffset++;
			//Unicode一文字追加
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
			UTF16DrawText.AddUTF16Char(kUChar_Space);
			uniOffset++;
			
			//
			tabletters++;
			
			//#473
			controlCodeStartUTF8Index.Add(originalTextIndex);
			controlCodeEndUTF8Index.Add(originalTextIndex+bc);
		}
		/*#844
		//==================SJISロスレスフォールバック文字の場合==================
		//#473 SJISロスレスフォールバック 2バイト目文字なしエラーを赤表示する
		else if( ucs4Char >= 0x105C00 && ucs4Char <= 0x105CFF )
		{
			//------------------オリジナルテキストに対応する配列にデータ設定------------------
			for( AIndex i = 0; i < bc; i++ )
			{
				//UTF16DrawText内の対応するindexを設定（今のunicode offset）
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				//タブ用文字カウント数を設定
				OriginalTextArray_TabLetters.Add(tabletters);
			}
			
			//エラー文字（1バイト目）取得
			AUChar	sjisch = (ucs4Char&0xFF);
			
			//エラーテキスト取得
			AText	t;
			t.SetLocalizedText("SJIS_Error_2ndByte");
			AText	text;
			text.Add(' ');
			text.Add(hex2asc((sjisch&0xF0)/0x10));
			text.Add(hex2asc(sjisch&0xF));
			text.Add(' ');
			text.AddText(t);
			text.ConvertFromUTF8ToUTF16();//#863
			//UTF16DrawTextにエラーテキスト追加
			UTF16DrawText.AddText(text);//#863
			//エラーテキストを設定
			for( AIndex i = 0; i < text.GetItemCount()/2; i++ )
			{
				//UTF16DrawTextにエラーテキスト追加した分の処理
				UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				uniOffset++;
			}
			
			//タブ
			tabletters++;
			
			//制御文字範囲設定
			controlCodeStartUTF8Index.Add(originalTextIndex);
			controlCodeEndUTF8Index.Add(originalTextIndex+bc);
		}
		*/
		//==================上記以外の文字の場合==================
		else
		{
			//#117
			if( gyoutou == true && inStartSpaceToIndent == true && inStartSpaceCount > 0 && inStartIndentWidth > 0 &&
						originalTextIndex+inStartSpaceCount <= lineInfo_LengthWithoutCR )
			{
				//現在のoriginalTextIndex位置からinStartSpaceCount分spaceが連続しているかどうかをチェックする
				ABool	nspace = true;
				for( AIndex i = originalTextIndex ; i < originalTextIndex+inStartSpaceCount; i++ )
				{
					AUChar	ch = (textp)[lineInfo_Start+i];//#733 inText.Get(lineInfo_Start+i);
					if( ch != kUChar_Space )
					{
						nspace = false;
						break;
					}
				}
				//n個のspaceが存在するならば、inStartIndentWidth個のspaceに置き換えて追加
				if( nspace == true )
				{
					startSpaceToIndentUTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
					//
					for( AIndex i = 0; i < inStartSpaceCount; i++ )
					{
						//方向キーによるキャレット移動で半角スペース分キャレット位置がうつるようにする
						AIndex	uo = uniOffset + i;
						if( uo >= uniOffset+inStartIndentWidth )   uo = uniOffset+inStartIndentWidth-1;
						OriginalTextArray_UnicodeOffset.Add(uo);
						OriginalTextArray_TabLetters.Add(tabletters);
						tabletters++;
					}
					//Unicode n文字追加
					for( AIndex i = 0; i < inStartIndentWidth; i++ )
					{
						//クリック時、左半分なら疑似タブ前、右半分なら疑似タブ後になるようにする
						if( i > inStartIndentWidth/2 )
						{
							UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex+inStartSpaceCount);
						}
						else
						{
							UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
						}
						UTF16DrawText.AddUTF16Char(kUChar_Space);
						uniOffset++;
					}
					//次のoriginalTextIndexの位置を設定
					nextOriginalTextIndex = originalTextIndex + inStartSpaceCount;
					nextUCS4CharLoaded = false;
					continue;
				}
			}
			
			//==================それ以外の普通の文字==================
			
			//------------------オリジナルテキストに対応する配列にデータ設定------------------
			for( AIndex i = 0; i < bc; i++ )
			{
				//UTF16DrawText内の対応するindexを設定（今のunicode offset）
				OriginalTextArray_UnicodeOffset.Add(uniOffset);
				//タブ用文字カウント数を設定
				OriginalTextArray_TabLetters.Add(tabletters);
			}
			
			/*
			//B0000たとえば"ﾃ"のみ記述されたファイルを読み込み、エンコーディングを誤認識した場合等、bcを足すとテキスト全体をオーバーする可能性がある
			→AText::Convert1CharToUCS4()にてチェックしているのでこのチェックはもう必要ない
			if( lineInfo_Start+originalTextIndex+bc > textcount )
			{
				ucs4Char = 0x20;
				bc = 1;
			}
			*/
			//
			if( ucs4Char < 0x10000 )
			{
				//inDisplayYenFor5Cがtrueなら、バックスラッシュ（U+005C）を半角￥（U+00A5)で表示する #940
				if( inDisplayYenFor5C == true && ucs4Char == 0x005C )
				{
					ucs4Char = 0x00A5;
				}
				//Unicode一文字追加
				UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				UTF16DrawText.AddUTF16Char(ucs4Char);
				uniOffset++;
			}
			else
			{
				//Unicodeサロゲートペア追加
				UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
				AUCS4Char	c = ucs4Char - 0x10000;
				AUCS4Char	high = (c&0x1FC00)>>10;
				UTF16DrawText.AddUTF16Char(0xD800+high);
				AUCS4Char	low = (c&0x3FF);
				UTF16DrawText.AddUTF16Char(0xDC00+low);
				uniOffset += 2;
			}
			/*
			//B0000 MacOSX10.4ではU+FFFF(EF BF BF)を使うとおかしくなる？ので、U+FFFFはU+3000(全角スペース)にする
			//★必要？
			if( ch == 0xEF )
			{
				if(	UTF8DrawText.Get(UTF8DrawText.GetItemCount()-2) == 0xBF &&
					UTF8DrawText.Get(UTF8DrawText.GetItemCount()-1) == 0xBF )
				{
					UTF8DrawText.Set(UTF8DrawText.GetItemCount()-3,0xE3);
					UTF8DrawText.Set(UTF8DrawText.GetItemCount()-2,0x80);
					UTF8DrawText.Set(UTF8DrawText.GetItemCount()-1,0x80);
				}
			}
			*/
			//
			if( unicodeData.IsWideWidth(ucs4Char) == false )//#414
			{
				tabletters++;
			}
			else
			{
				tabletters += tabinc0800;
			}
		}
	}
	UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
	//inTextの文字数+1まで、OriginalTextArray_xxxを設定する #0 改行のみのテキストの場合に最終文字後の位置にOriginalTextArray_UnicodeOffsetデータが無い問題の修正のため、1回実行からループ実行へ変更
	//・・・しようと思ったが、なぜかすごく遅くなる（行折り返し計算プログレスダイアログが出るので元に戻す。）
	//while( OriginalTextArray_UnicodeOffset.GetItemCount() < lineInfo_LengthWithoutCR+1 )
	{
		OriginalTextArray_UnicodeOffset.Add(uniOffset);
		OriginalTextArray_TabLetters.Add(tabletters);
	}

	/*
	if( inGetUTF16Text == true )
	{
		//UTF8DrawTextからUTF16DrawTextへ変換
		UTF16DrawText.SetText(UTF8DrawText);
		UTF16DrawText.ConvertFromUTF8ToUTF16();
		//
		while( UTF16DrawTextArray_OriginalTextIndex.GetItemCount() <= UTF16DrawText.GetItemCount()/2 )
		{
			UTF16DrawTextArray_OriginalTextIndex.Add(originalTextIndex);
		}
	}
	*/
}

//R0240
/**
テキスト描画用データ生成
*/
void	CTextDrawData::MakeTextDrawDataWithoutStyle( const AText& inText )
{
	MakeTextDrawDataWithoutStyle(inText,4,0,true,true,false,0,inText.GetItemCount());
}

//#532
/**
追加
*/
void	CTextDrawData::AddText( const AText& inUTF8Text, const AText& inUTF16Text, const AColor& inColor )
{
	//最後のデータを取得
	AIndex	index = UTF16DrawTextArray_OriginalTextIndex.Get(UTF16DrawTextArray_OriginalTextIndex.GetItemCount()-1);
	AIndex	uniOffset = OriginalTextArray_UnicodeOffset.Get(OriginalTextArray_UnicodeOffset.GetItemCount()-1);
	AItemCount	tabletters = OriginalTextArray_TabLetters.Get(OriginalTextArray_TabLetters.GetItemCount()-1);
	
	//最後の１つを消す
	UTF16DrawTextArray_OriginalTextIndex.Delete1(UTF16DrawTextArray_OriginalTextIndex.GetItemCount()-1);
	OriginalTextArray_UnicodeOffset.Delete1(OriginalTextArray_UnicodeOffset.GetItemCount()-1);
	OriginalTextArray_TabLetters.Delete1(OriginalTextArray_TabLetters.GetItemCount()-1);
	
	//色設定
	attrPos.Add(uniOffset);
	attrColor.Add(inColor);
	attrStyle.Add(kTextStyle_Normal);
	
	//UTF16テキスト設定
	UTF16DrawText.AddText(inUTF16Text);
	
	//UTF8-UTF16 インデックス変換配列設定
	for( AIndex pos = 0; pos < inUTF8Text.GetItemCount(); )
	{
		AItemCount	bc = inUTF8Text.GetUTF8ByteCount(pos);
		for( AIndex i = 0; i < bc; i++ )
		{
			OriginalTextArray_UnicodeOffset.Add(uniOffset);
			OriginalTextArray_TabLetters.Add(tabletters);
		}
		pos+= bc;
		uniOffset++;
		UTF16DrawTextArray_OriginalTextIndex.Add(index);
		index++;
		tabletters++;
	}
	OriginalTextArray_UnicodeOffset.Add(uniOffset);
	UTF16DrawTextArray_OriginalTextIndex.Add(index);
	OriginalTextArray_TabLetters.Add(tabletters);
	//
	//#1034 ClearTextLayout();
	
	//CTLine解放 #1034
	ClearCTLineRef();
	
	//CTTypesetter解放 #1034
	ClearCTTypesetterRef();
	
}

//#1316
/**
Attributeを後から追加する
*/
void	CTextDrawData::AddAttributeWithUTF8Offset( const AIndex inStart, const AIndex inEnd, const AColor& inColor )
{
	AIndex	spos = OriginalTextArray_UnicodeOffset.Get(inStart);
	AIndex	epos = OriginalTextArray_UnicodeOffset.Get(inEnd);
	additionalAttrPos.Add(spos);
	additionalAttrLength.Add(epos-spos);
	additionalAttrColor.Add(inColor);
	additionalAttrStyle.Add(kTextStyle_Normal);
	
	//CTLine解放 #1034
	ClearCTLineRef();
	
	//CTTypesetter解放 #1034
	ClearCTTypesetterRef();
}

/**
Attributeを後から追加する
*/
void	CTextDrawData::AddAttributeWithUnicodeOffset( const AIndex inStart, const AIndex inEnd, const AColor& inColor )
{
	additionalAttrPos.Add(inStart);
	additionalAttrLength.Add(inEnd-inStart);
	additionalAttrColor.Add(inColor);
	additionalAttrStyle.Add(kTextStyle_Normal);
	
	//CTLine解放 #1034
	ClearCTLineRef();
	
	//CTTypesetter解放 #1034
	ClearCTTypesetterRef();
}

