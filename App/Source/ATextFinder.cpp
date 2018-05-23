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

ATextFinder

*/

#include "stdafx.h"

#include "ATextFinder.h"
#include "AApp.h"

#pragma mark ===========================
#pragma mark [クラス]ATextFinder
#pragma mark ===========================
//テキスト検索

#pragma mark ---コンストラクタ／デストラクタ
//コンストラクタ
ATextFinder::ATextFinder( AObjectArrayItem* inParent )//#750
: AObjectArrayItem(inParent),mCurrentParameter_IgnoreCase(true),mCurrentParameter_IgnoreBackslashYen(true),
mCurrentParameter_Fuzzy_NFKD(true),mCurrentParameter_Fuzzy_KanaType(true),
mCurrentParameter_Fuzzy_Onbiki(true),mCurrentParameter_Fuzzy_KanaYuragi(true)
,mFindTextLen(0)//#733
,mCurrentParameter_Reverse(false)//#789
,mInited(false)
{
}

/**
初期化（BMテーブルの初期化）
*/
void	ATextFinder::Init()
{
	//BMTable初期化
	mBMTable.DeleteAll();
	mBMTable.Reserve(0,kBMTableCount);//#852
	for( AIndex i = 0; i < kBMTableCount; i++ )
	{
		//#852 mBMTable.Add(0);
		mBMTable.Set(i,0);//#852
	}
	//初期化フラグ設定
	mInited = true;
}

#pragma mark ===========================

#pragma mark ---検索実行

//検索実行
ABool	ATextFinder::ExecuteFind( const AText& inText, const AFindParameter& inParam, const ABool inReverse,//#789
		const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos,
		const ABool& inAbortFlag, //B0359
		const ABool inUseModalSession, ABool& outModalSessionAborted )
{
	//未初期化なら初期化実行
	if( mInited == false )
	{
		Init();
	}
	//
	if( inParam.regExp == false )
	{
		return BMFind(inText,inParam,inReverse,inSpos,inEpos,outSpos,outEpos,inAbortFlag,inUseModalSession,outModalSessionAborted);
	}
	else
	{
		return RegExpFind(inText,inParam,inReverse,inSpos,inEpos,outSpos,outEpos,inAbortFlag,inUseModalSession,outModalSessionAborted);
	}
}

//前方向検索用バッファ #789
AUCS4Char	gFindTextBufferForReverse[kBufferSize_TextFinder_UCS4FindText];

//BM法による通常検索
ABool	ATextFinder::BMFind( const AText& inText, const AFindParameter& inParam, const ABool inReverse,//#789
		const AIndex inSpos, const AIndex inEpos, AIndex& outSpos, AIndex& outEpos,
		const ABool& inAbortFlag, //B0359
		const ABool inUseModalSession, ABool& outModalSessionAborted )
{
	//中断されたフラグ初期化
	outModalSessionAborted = false;
	
	//#789 検索文字列、検索対象テキストの長さが0ならここでリターン
	if( inParam.findText.GetItemCount() <= 0 )
	{
		_ACError("",NULL);
		return false;
	}
	if( inText.GetItemCount() <= 0 )
	{
		return false;
	}
	
	ABool	result = false;
	
	//#733 高速化
	//対象テキストへのポインタ取得
	AStTextPtr	targetStTextPtr(inText,0,inText.GetItemCount());
	const AUChar*	targettextptr = targetStTextPtr.GetPtr();
	const AItemCount	targettextlen = targetStTextPtr.GetByteCount();
	//検索文字列へのポインタ取得
	AStTextPtr	findStTextPtr(inParam.findText,0,inParam.findText.GetItemCount());
	const AUChar*	findtextptr = findStTextPtr.GetPtr();
	const AItemCount	findtextlen = findStTextPtr.GetByteCount();
	
	//#166 曖昧検索機能強化
	ABool	fuzzy_NFKD = false;
	ABool	fuzzy_KanaType = false;
	ABool	fuzzy_Onbiki = false;
	ABool	fuzzy_KanaYuragi = false;
	if( inParam.fuzzy == true )
	{
		fuzzy_NFKD 			= GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFuzzyFind_NFKD);
		fuzzy_KanaType 		= GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFuzzyFind_KanaType);
		fuzzy_Onbiki		= GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFuzzyFind_Onbiki);
		fuzzy_KanaYuragi 	= GetApp().NVI_GetAppPrefDB().GetData_Bool(AAppPrefDB::kFuzzyFind_KanaYuragi);
	}
	
	//#709
	//スペース・タブ・改行、音引き無視の場合に、
	//全ての検索対象文字が無視対象文字の場合は、スペース・タブ・改行、音引き無視しないようにする
	//たとえば「ー」だけを検索する場合。検索対象が「ー」なら曖昧検索をする必要は無いはず。
	//（mFindTextが空になるのを防ぐ。）
	ABool	ignoreSpace = inParam.ignoreSpaces;
	ABool	ignoreOnbiki = fuzzy_Onbiki;
	ABool	targetCharExists = false;//無視しない文字有りフラグ
	//検索文字列内を検索
	for( AIndex pos = 0; pos < inParam.findText.GetItemCount(); )
	{
		//UCS4文字に変換
		AUCS4Char	uc = 0;
		pos += AText::Convert1CharToUCS4(findtextptr,findtextlen,pos,uc);//#733
		
		//無視対象文字でなければフラグonにしてbreak
		if( IsIgnoreChar(uc,ignoreSpace,ignoreOnbiki) == false )
		{
			targetCharExists = true;
			break;
		}
	}
	//全ての文字が無視対象文字の場合、スペース・タブ・改行、音引き無視しないようにする
	if( targetCharExists == false )
	{
		ignoreSpace = false;
		ignoreOnbiki = false;
	}
	
	//#733
	//高速化のため、参照をループ外で取得し記憶
	AUnicodeData&	unicodeData = GetApp().NVI_GetUnicodeData();
	const AHashArray<AUCS4Char>&	fuzzySearchNormalizeArray_from = GetApp().SPI_GetFuzzySearchNormalizeArray_from();
	const AObjectArray< AArray<AUCS4Char> >&	fuzzySearchNormalizeArray_to = GetApp().SPI_GetFuzzySearchNormalizeArray_to();
	
	//=======================================================
	//　1. まず、mFindTextにUTF16で検索文字列を格納し、BMTable構成する
	//=======================================================
	//前回検索したときのパラメータと比較し、全く同じであれば、mFindText, mBMTableはそのまま使用する
	if( 	(inParam.findText.Compare(mCurrentParameter_FindText) == false) ||
			(inParam.ignoreCase != mCurrentParameter_IgnoreCase) ||
	   (inParam.ignoreBackslashYen != mCurrentParameter_IgnoreBackslashYen) ||
	   (inParam.ignoreSpaces != mCurrentParameter_IgnoreSpaces ) ||//#936
			//#166 (inParam.fuzzy != mCurrentParameter_Fuzzy) )
			//#166
			(fuzzy_NFKD != mCurrentParameter_Fuzzy_NFKD) ||
			(fuzzy_KanaType != mCurrentParameter_Fuzzy_KanaType) ||
			(fuzzy_Onbiki != mCurrentParameter_Fuzzy_Onbiki) ||
			(fuzzy_KanaYuragi != mCurrentParameter_Fuzzy_KanaYuragi) ||
			(inReverse != mCurrentParameter_Reverse) )//#789
	{
		//今回検索のパラメータを保存
		mCurrentParameter_FindText.SetText(inParam.findText);
		mCurrentParameter_IgnoreCase = inParam.ignoreCase;
		mCurrentParameter_IgnoreBackslashYen = inParam.ignoreBackslashYen;
		mCurrentParameter_IgnoreSpaces = inParam.ignoreSpaces;//#936
		//#166 mCurrentParameter_Fuzzy = inParam.fuzzy;
		//#166
		mCurrentParameter_Fuzzy_NFKD = fuzzy_NFKD;
		mCurrentParameter_Fuzzy_KanaType = fuzzy_KanaType;
		mCurrentParameter_Fuzzy_Onbiki = fuzzy_Onbiki;
		mCurrentParameter_Fuzzy_KanaYuragi = fuzzy_KanaYuragi;
		//#789 逆方向フラグ
		mCurrentParameter_Reverse = inReverse;
		
		//===========================================================
		//0～GetItemCount()の間の文字をUCS4に変換して、mFindTextに格納する。
		//inReverseがtrueの場合は、後ろからmFindTextに格納する。
		//===========================================================
		
		//#733 mFindText.DeleteAll();
		mFindTextLen = 0;//#733
		//findTextの開始位置の取得 #789
		//順方向検索の場合は、最初のUTF-8文字の開始位置を、開始位置にする
		AIndex	pos = 0;
		if( inReverse == true )
		{
			//逆方向検索の場合は、最後のUTF-8文字の開始位置を、開始位置にする。
			pos = inParam.findText.GetCurrentCharPos(inParam.findText.GetItemCount()-1);
		}
		//
		//#789 for( AIndex pos = 0; pos < inParam.findText.GetItemCount(); )
		while( pos >= 0 && pos < inParam.findText.GetItemCount() )//#789 posがfindTextの有効範囲内の間、ループする
		{
			//posの位置のUnicode文字を取得
			AUCS4Char	uc = 0;//#412
			if( inReverse == false )
			{
				//順方向検索の場合は、posの位置のUnicode文字を取得し、次のUTF-8文字開始位置へ移動。
				pos += AText::Convert1CharToUCS4(findtextptr,findtextlen,pos,uc);//#412 #733
			}
			else//#789
			{
				//逆方向検索の場合は、posの位置のUnicode文字を取得
				AText::Convert1CharToUCS4(findtextptr,findtextlen,pos,uc);
				//posを前のUTF-8文字開始位置へ移動（ただし、pos=0ならpos=-1になる）
				pos = inParam.findText.GetPrevCharPos(pos,true);
			}
			
			//#166
			//曖昧検索　スペース文字、音引き無視
			if( IsIgnoreChar(uc,ignoreSpace,ignoreOnbiki) == true )
			{
				continue;
			}
			
			//正規化して追加 #166
			//#427
			AUnicodeNormalizeType	normalizeType = kUnicodeNormalizeType_D;//#863 kUnicodeNormalizeType_None;
			//
			if( fuzzy_NFKD == true )
			{
				normalizeType = kUnicodeNormalizeType_KD;
			}
			//mFindTextへ、ucを正規化したUnicode文字を追加
			if( inReverse == false )
			{
				//------------------次方向検索の場合------------------
				if( unicodeData.CatNormalizedText(
												  mFindText,mFindTextLen,kBufferSize_TextFinder_UCS4FindText,uc,
												  normalizeType,inParam.ignoreCase,fuzzy_KanaType,inParam.ignoreBackslashYen,fuzzy_KanaYuragi,
												  fuzzySearchNormalizeArray_from,fuzzySearchNormalizeArray_to) == false )
				{
					_ACError("",this);
					return false;
				}
			}
			else
			{
				//------------------前方向検索の場合------------------
				//#789
				AItemCount	len = 0;
				if( unicodeData.CatNormalizedText(
												  gFindTextBufferForReverse,len,kBufferSize_TextFinder_UCS4FindText,uc,
												  normalizeType,inParam.ignoreCase,fuzzy_KanaType,inParam.ignoreBackslashYen,fuzzy_KanaYuragi,
												  fuzzySearchNormalizeArray_from,fuzzySearchNormalizeArray_to) == false )
				{
					_ACError("",this);
					return false;
				}
				//前方向検索の場合、反転したテキストをmFindTextに格納
				for( AIndex i = len-1; i >= 0; i-- )
				{
					mFindText[mFindTextLen] = gFindTextBufferForReverse[i];
					mFindTextLen++;
				}
			}
		}
		//BMTable初期化
		for( AIndex i = 0; i < kBMTableCount; i++ )
		{
			mBMTable.Set(i,0);
		}
		//BMTable作成
		for( AIndex pos = 0; pos < /*#733 mFindText.GetItemCount()-1*/mFindTextLen-1; pos++ )
		{
			AUCS4Char	uc = mFindText[pos];//#733 .Get(pos);
			//#166 BMテーブル範囲内の場合のみ位置データ格納する（範囲外データは最後のスライド時に、BMTable参照せずに+1する）
			if( uc < kBMTableCount )
			{
				//BMTableに、各文字について、findTextの中で一番最後に現れる位置+1を格納
				mBMTable.Set(uc,pos+1);
			}
		}
	}
	
	//=======================================================
	//　　2. 検索メインループ
	//=======================================================
	//srcTextの各文字に対応する元テキスト(inText)の位置データ
	//#733 srcText関連(srcText,srcTextPos,srcTextPosDecomp)のlengthはsrcTextLenを使う。
	AItemCount	srcTextLen = 0;//#733
	//#733 AArray<AIndex>	srcTextPos;
	AIndex	srcTextPos[kBufferSize_TextFinder_UCS4FindText];//#733
	//元のテキスト(inText)の一文字が、srcText複数の文字になることがあるので、一文字内のオフセットを記憶
	//#733 AArray<AIndex>	srcTextPosDecomp;
	AIndex	srcTextPosDecomp[kBufferSize_TextFinder_UCS4FindText];//#733
	//検索ターゲットテキスト（元のテキスト(inText)から正規化しながら取り出す）
	//#733 AArray<AUCS4Char>	srcText;//#412
	AUCS4Char	srcText[kBufferSize_TextFinder_UCS4FindText];//#733
	AIndex	srcSposDecompPos = 0;
	//#789 for( AIndex srcSpos = inSpos; srcSpos < inEpos; )
	//検索対象テキストの開始位置の取得 #789
	//順方向検索の場合は、inSposから開始
	AIndex	srcSpos = inSpos;
	if( inReverse == true )
	{
		//逆方向検索の場合は、inEposの位置の1つ前のUTF-8文字から開始
		srcSpos = inText.GetCurrentCharPos(inEpos-1);
	}
	while( srcSpos >= inSpos && srcSpos < inEpos )//#789 inTextのinSpos～inEposの間にsrcSposがある間、ループ
	{
		//B0359
		if( inAbortFlag == true )
		{
			break;
		}
		//モーダルセッション継続チェック
		if( inUseModalSession == true )
		{
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Find,0,false,srcSpos-inSpos,inEpos-inSpos) == false )
			{
				outModalSessionAborted = true;
				break;
			}
		}
		
		//#733 srcTextPos.DeleteAll();
		//#733 srcTextPosDecomp.DeleteAll();
		//#733 srcText関連のlengthはsrcTextLenを使う。srcTextLenはこのすぐ下で0に初期化。
		
		ABool	comparecheck = true;
		//=======================================================
		//　2.1. 検索するテキストをUnicodeに変換しながらfindTextLen分だけ、srcTextに格納
		//=======================================================
		//#733 srcText.DeleteAll();
		srcTextLen = 0;//#733
		AIndex	srcLastCharPos = srcSpos;
		AIndex	decompDelete = srcSposDecompPos;
		AItemCount	findTextItemCount = mFindTextLen;//#733 mFindText.GetItemCount();
		for( ; srcTextLen < findTextItemCount;  )//srcTextの長さがmFindTextの長さになるまでループ
		{
			//途中でinEpos/inSposに到達した場合は残りは0xFFFFで埋める
			if( srcLastCharPos >= inEpos || srcLastCharPos < inSpos )
			{
				if( srcTextLen >= kBufferSize_TextFinder_UCS4FindText )
				{
					_ACThrow("",NULL);
				}
				else
				{
					//srcTextに0xFFFFを追加
					srcTextPos[srcTextLen] = srcLastCharPos;
					srcTextPosDecomp[srcTextLen] = 0;
					srcText[srcTextLen] = 0xFFFF;
					srcTextLen++;
				}
				continue;
			}
			
			//ucにposの位置のUCS4文字を取得
			AUCS4Char	uc = 0;//#412
			AItemCount	bc = AText::Convert1CharToUCS4(targettextptr,targettextlen,srcLastCharPos,uc);//#412 #733
			//cursrctextposに今のucの対象テキスト内位置を記憶 #789
			AIndex	cursrctextpos = srcLastCharPos;
			//srcLastCharPosを移動
			if( inReverse == false )
			{
				//順方向検索なら、次の文字の開始位置へ移動
				srcLastCharPos += bc;
			}
			else//#789
			{
				//逆方向検索なら、前の文字の開始位置へ移動
				srcLastCharPos = inText.GetPrevCharPos(srcLastCharPos,true);
			}
			
			//#166
			//曖昧検索　スペース文字、音引き無視（srcTextへ格納しない）
			while( IsIgnoreChar(uc,ignoreSpace,ignoreOnbiki) == true )
			{
				bc = 0;//#477
				//途中でinEpos/inSposに到達した場合はそこで終了
				if( srcLastCharPos >= inEpos || srcLastCharPos < inSpos )   break;
				
				//次の文字読み込み
				bc = AText::Convert1CharToUCS4(targettextptr,targettextlen,srcLastCharPos,uc);//#733
				//cursrctextposに今のucの対象テキスト内位置を記憶 #789
				cursrctextpos = srcLastCharPos;
				//
				if( inReverse == false )
				{
					//順方向検索なら、次の文字の開始位置へ移動
					srcLastCharPos += bc;
				}
				else//#789
				{
					//逆方向検索なら、前の文字の開始位置へ移動
					srcLastCharPos = inText.GetPrevCharPos(srcLastCharPos,true);
				}
			}
			//スペース文字等を読み飛ばし途中でinEposに到達した場合はそこで終了（FFFF埋め→必ず一致しない）
			if( bc == 0 && (srcLastCharPos >= inEpos || srcLastCharPos < inSpos) )   continue;//#477 #789
			
			//各Unicode文字に対応するtext posをcursrctextposに格納する #166 上から移動、かつ、srcLastCharPosではなく-bcにする（対象文字の前）
			if( srcTextLen >= kBufferSize_TextFinder_UCS4FindText )
			{
				_ACThrow("",NULL);
			}
			else
			{
				//対象テキストの該当位置を記憶する。
				//※srcTextLenの変更は下のCatNormalizedText()で実行される
				//※対象テキストの一文字が、複数の文字になってsrcTextに保存されることがあるが、下のfor文で格納される。
				srcTextPos[srcTextLen] = cursrctextpos;//#789 srcLastCharPos-bc;
				srcTextPosDecomp[srcTextLen] = 0;//decompの0文字目を意味する
			}
			//srcTextに格納した(decompの)最初の文字の位置を記憶
			AIndex	firstDecompPos = srcTextLen;//#733 srcText.GetItemCount();
			
			//正規化して追加 #166
			//#427
			AUnicodeNormalizeType	normalizeType = kUnicodeNormalizeType_D;//#863 kUnicodeNormalizeType_None;
			//
			if( fuzzy_NFKD == true )
			{
				normalizeType = kUnicodeNormalizeType_KD;
			}
			//
			if( inReverse == false )
			{
				//------------------次方向検索の場合------------------
				if( unicodeData.CatNormalizedText(
												  srcText,srcTextLen,kBufferSize_TextFinder_UCS4FindText,uc,
												  normalizeType,inParam.ignoreCase,fuzzy_KanaType,inParam.ignoreBackslashYen,fuzzy_KanaYuragi,
												  fuzzySearchNormalizeArray_from,fuzzySearchNormalizeArray_to) == false )
				{
					_ACError("",this);
					return false;
				}
			}
			else
			{
				//------------------前方向検索の場合------------------
				//#789
				AItemCount	len = 0;
				if( unicodeData.CatNormalizedText(
												  gFindTextBufferForReverse,len,kBufferSize_TextFinder_UCS4FindText,uc,
												  normalizeType,inParam.ignoreCase,fuzzy_KanaType,inParam.ignoreBackslashYen,fuzzy_KanaYuragi,
												  fuzzySearchNormalizeArray_from,fuzzySearchNormalizeArray_to) == false )
				{
					_ACError("",this);
					return false;
				}
				//前方向検索の場合、反転したテキストをsrcTextに格納
				for( AIndex i = len-1; i >= 0; i-- )
				{
					srcText[srcTextLen] = gFindTextBufferForReverse[i];
					srcTextLen++;
				}
			}
			
			// 曖昧検索の場合は元のテキストの一文字が複数の文字になることがあるので、srcTextPos, srcTextPosDecompを追加する。
			for( AIndex i = firstDecompPos+1; i < srcTextLen; i++ )
			{
				srcTextPos[i] = srcTextPos[firstDecompPos];//最初のdecomp文字と同じ対象テキスト内位置を格納//#733 .Get(firstDecompPos);
				srcTextPosDecomp[i] = i-firstDecompPos;//decomp文字のindex(1,2,...)を格納
			}
			//曖昧検索の場合はもとのテキストの一文字の途中から開始することがあるので、最初の数文字を削除
			if( decompDelete > 0 )
			{
				//decompDelete分だけずらして、lenを減らす
				for( AIndex dstPos = 0; dstPos < srcTextLen-decompDelete; dstPos++ )
				{
					srcText[dstPos] = srcText[dstPos+decompDelete];
					srcTextPos[dstPos] = srcTextPos[dstPos+decompDelete];
					srcTextPosDecomp[dstPos] = srcTextPosDecomp[dstPos+decompDelete];
				}
				srcTextLen -= decompDelete;
				//
				decompDelete = 0;
			}
		}
		//#733 srcText.Add(0);
		//#733 srcTextPos.Add(srcLastCharPos);
		//#733 srcTextPosDecomp.Add(0);
		//最後に0を追加
		if( srcTextLen >= kBufferSize_TextFinder_UCS4FindText )
		{
			_ACThrow("",NULL);
		}
		else
		{
			srcText[srcTextLen] = 0;
			srcTextPos[srcTextLen] = srcLastCharPos;
			srcTextPosDecomp[srcTextLen] = 0;
			srcTextLen++;
		}
		
		//=======================================================
		//　2.2. 比較
		//=======================================================
		for( AIndex i = 0; i < findTextItemCount; i++ )
		{
			if( mFindText[i] != srcText[i] )//#733 mFindText.Get(i) != srcText.Get(i) )
			{
				comparecheck = false;
				break;
			}
		}
		
		if( comparecheck == true )
		{
			ABool	wholecheck = true;
			if( inParam.wholeWord )
			{
				//一致文字列の最初の文字と、その１つ前の文字が両方英数字ならWholeWordチェックNG
				//#789 if( srcSpos-1 >= 0 )
				//#789 {
				//#789 AUChar	ch1 = inText.Get(srcSpos-1);
				//#789 AUChar	ch2 = inText.Get(srcSpos);
				{
					AIndex	p = -1;
					if( inReverse == false )
					{
						//順方向の場合は、srcSposの前の位置を取得
						p = inText.GetPrevCharPos(srcSpos,true);
					}
					else
					{
						//逆方向の場合は、srcSposの次の位置を取得
						p = inText.GetNextCharPos(srcSpos);
					}
					//srcSpos, p両方ともinTextの範囲内の場合のみ、whole wordチェック
					if( srcSpos >= 0 && srcSpos < inText.GetItemCount() &&
								p >= 0 && p < inText.GetItemCount() )
					{
						//UCS4文字取得
						AUCS4Char	ch1 = 0, ch2 = 0;
						AText::Convert1CharToUCS4(targettextptr,targettextlen,srcSpos,ch1);
						AText::Convert1CharToUCS4(targettextptr,targettextlen,p,ch2);
						//両方ともアルファベットならwhole wordでないと判断
						//#416 if( Macro_IsAlphabet(ch1) == true && Macro_IsAlphabet(ch2) == true )
						if( unicodeData.IsAlphabetOrNumberOrUnderbar(ch1) == true && unicodeData.IsAlphabetOrNumberOrUnderbar(ch2) == true )//#416
						{
							wholecheck = false;
						}
					}
					/*#416 今までと仕様が変わってしまうのでとりあえず保留
					AUCS4Char	ch1 = 0, ch2 = 0;
					inText.Convert1CharToUCS4(inText.GetPrevCharPos(srcSpos),ch1);
					inText.Convert1CharToUCS4(srcSpos,ch2);
					if( GetApp().NVI_GetUnicodeData().IsAlphabetOrNumber(ch1) == true &&
					GetApp().NVI_GetUnicodeData().IsAlphabetOrNumber(ch2) == true )
					{
					wholecheck = false;
					}
					*/
				}
				//一致文字列の最後の文字と、その１つあとの文字が両方英数字ならWholeWordチェックNG
				//#789 if( srcLastCharPos < inText.GetItemCount() )
				//#789 {
				//#789 AUChar	ch1 = inText.Get(srcLastCharPos-1);
				//#789 AUChar	ch2 = inText.Get(srcLastCharPos);
				{
					AIndex	p = -1;
					if( inReverse == false )
					{
						//順方向の場合は、srcLastCharPosの前の位置を取得
						p = inText.GetPrevCharPos(srcLastCharPos,true);
					}
					else
					{
						//逆方向の場合は、srcLastCharPosの次の位置を取得
						p = inText.GetNextCharPos(srcLastCharPos);
					}
					//srcLastCharPos, p両方ともinTextの範囲内の場合のみ、whole wordチェック
					if( srcLastCharPos >= 0 && srcLastCharPos < inText.GetItemCount() &&
								p >= 0 && p < inText.GetItemCount() )
					{
						//UCS4文字取得
						AUCS4Char	ch1 = 0, ch2 = 0;
						AText::Convert1CharToUCS4(targettextptr,targettextlen,srcLastCharPos,ch1);
						AText::Convert1CharToUCS4(targettextptr,targettextlen,p,ch2);
						//両方ともアルファベットならwhole wordでないと判断
						//#416 if( Macro_IsAlphabet(ch1) == true && Macro_IsAlphabet(ch2) == true )
						if( unicodeData.IsAlphabetOrNumberOrUnderbar(ch1) == true && unicodeData.IsAlphabetOrNumberOrUnderbar(ch2) == true )//#416
						{
							wholecheck = false;
						}
					}
					/*#416 今までと仕様が変わってしまうのでとりあえず保留
					AUCS4Char	ch1 = 0, ch2 = 0;
					inText.Convert1CharToUCS4(inText.GetPrevCharPos(srcLastCharPos),ch1);
					inText.Convert1CharToUCS4(srcLastCharPos,ch2);
					if( GetApp().NVI_GetUnicodeData().IsAlphabetOrNumber(ch1) == true &&
					GetApp().NVI_GetUnicodeData().IsAlphabetOrNumber(ch2) == true )
					{
					wholecheck = false;
					}
					*/
				}
			}
			if( wholecheck )
			{
				//一致して、wholeWordチェックもOKならそこで終了
				outSpos = srcSpos;
				outEpos = srcLastCharPos;
				//#166 無視文字以外の部分からを開始位置とするため
				if( /*#733 srcTextPos.GetItemCount()*/srcTextLen > 0 )
				{
					outSpos = srcTextPos[0];//#733.Get(0);
				}
				//#789
				//逆方向の場合は、srcSpos:最後の文字の開始位置、srcLastCharPos:最初の文字の前の文字の開始位置となっているので、
				//補正する。
				if( inReverse == true )
				{
					AIndex	tmp = inText.GetNextCharPos(outSpos);
					outSpos = inText.GetNextCharPos(outEpos);
					outEpos = tmp;
				}
				//検索終了
				result = true;
				break;
			}
		}
		//=======================================================
		//　2.3. BM法により次のマッチング開始位置を決定する
		//=======================================================
		//1.srcTextの最後の文字に相当するBMTableの値を取得（BMTableの値は0～findTextLen-1。その文字が最後に現れる位置（基数1））
		//2.BMTableの値が0の場合：srcLastCharPos（findTextLenに相当する文字全てを読み終わった位置）へ移動
		//　BMTableの値が1～findTextLen-1の場合：findTextLen-bmValue（スライドする文字数）に相当する位置へ移動
		AUCS4Char	bmChar = srcText[findTextItemCount-1];//srcTextの最後の文字 #733 srcText.Get(findTextItemCount-1);
		if( bmChar < kBMTableCount )//#166
		{
			//BMTableの値を取得
			AIndex	bmValue = mBMTable.Get(bmChar);
			if( bmValue == 0 )//比較対象先テキストの最後の文字のBMTableが0、すなわち、検索文字列内にその文字がない
			{
				//srcTextの最後の文字が、検索文字列内にない場合、そのsrcTextの範囲内で一致する可能性はないので、
				//srcTextの次の文字から、次の検索を行う。
				srcSpos = srcTextPos[findTextItemCount];//#733.Get(findTextItemCount);
				srcSposDecompPos = srcTextPosDecomp[findTextItemCount];//#733 .Get(findTextItemCount);
			}
			else
			{
				//次に一致する可能性がある位置、すなわち、srcTextの(findTextLen-bmValue)文字目の位置へ移動する。
				AIndex	nextSrcSpos = srcTextPos[findTextItemCount-bmValue];//#733 .Get(findTextItemCount-bmValue);
				AIndex	nextSrcSposDecompPos = srcTextPosDecomp[findTextItemCount-bmValue];//#733 .Get(findTextItemCount-bmValue);
				if( !((nextSrcSpos == srcSpos)&&(nextSrcSposDecompPos == srcSposDecompPos)) )
				{
					srcSpos = nextSrcSpos;
					srcSposDecompPos = nextSrcSposDecompPos;
				}
				else
				{
					//万一、次の開始位置が、今回の開始位置と同じだった場合、無限ループしないように、次／前の文字へ移動
					if( inReverse == false )
					{
						//順方向の場合は、次の文字へ移動
						srcSpos = inText.GetNextCharPos(srcSpos);
					}
					else//#789
					{
						//逆方向の場合は、前の文字へ移動
						srcSpos = inText.GetPrevCharPos(srcSpos,true);
					}
					srcSposDecompPos = 0;
					_ACError("Find BMTable Error?",NULL);
				}
			}
		}
		else
		{
			//bmCharがBMTable範囲外の場合、+1だけスライドさせる #166
			if( inReverse == false )
			{
				//順方向の場合は、次の文字へ移動
				srcSpos = inText.GetNextCharPos(srcSpos);
			}
			else//#789
			{
				//逆方向の場合は、前の文字へ移動
				srcSpos = inText.GetPrevCharPos(srcSpos,true);
			}
			srcSposDecompPos = 0;
		}
	}
	//#709
	//検索結果有り、かつ、音引き無視、かつ、検索ヒット文字の終了位置がテキストの最後でない場合、
	//検索ヒット文字の終了位置直後の音引きは検索結果範囲に含める
	if( result == true && ignoreOnbiki == true && outEpos < inText.GetItemCount() )
	{
		//検索ヒット文字の終了位置直後のUCS4文字を取得
		AUCS4Char	uc = 0;
		AItemCount	len = AText::Convert1CharToUCS4(targettextptr,targettextlen,outEpos,uc);//#733
		if( IsIgnoreChar(uc,false,true) == true )
		{
			//音引き文字なら、その文字分だけ、終了位置をずらす。
			outEpos += len;
		}
	}
	//
	return result;
	
}

//正規表現検索
ABool	ATextFinder::RegExpFind( const AText& inText, const AFindParameter& inParam, const ABool inReverse,//#789
		const AIndex inSpos, const AIndex inEpos, 
		AIndex& outSpos, AIndex& outEpos, const ABool& inAbortFlag, //B0359
		const ABool inUseModalSession, ABool& outModalSessionAborted )
{
	//中断したフラグを初期化
	outModalSessionAborted = false;
	
	//
	AText	findText;
	findText.SetText(inParam.findText);
	/*#501 ARegExp::SetRE()内で処理するように変更
	AText	yenText;
	yenText.SetLocalizedText("Text_Yen");
	AText	backslashText;
	backslashText.Add(kUChar_Backslash);
	findText.ReplaceText(yenText,backslashText);
	*/
	try//B0317このtry,catchは削除してもいいとは思う
	{
		mRegExp.SetRE(findText,inParam.ignoreCase);//#930
	}
	catch(...)
	{
		ASoundWrapper::Beep();
		return false;
	}
	
	//B0056 行全体を選択した場合に、次の行の行頭が^に一致してしまう問題対策
	/*if( inEnd.h == 0 && inEnd.v > 0 )
	{
		if( mText->GetChar(inepos-1) == '\r' )   inepos--;
	}*///→対策間違い　行全体を選択して、その行の最後のリターンコードが\rに一致しない(B0083)
	//B0056はURegExp::MatchRecursiveにて再修正
	
	ABool	result = false;
	//#789 for( AIndex spos = inSpos; spos <= inEpos; spos = inText.GetNextCharPos(spos) ) //B0047 spos<ineposを<=に変更
	//開始位置取得
	//順方向検索の場合、inSposの位置から開始
	AIndex	spos = inSpos;
	if( inReverse == true )
	{
		//逆方向検索の場合、inEposの位置から開始
		spos = inEpos;
	}
	//posがinSpos～inEposの間、ループする
	while( spos >= inSpos && spos <= inEpos )
	{
		//B0359
		if( inAbortFlag == true )
		{
			break;
		}
		//モーダルセッション継続チェック
		if( inUseModalSession == true )
		{
			if( GetApp().SPI_CheckContinueingEditProgressModalSession(kEditProgressType_Find,0,false,spos-inSpos,inEpos-inSpos) == false )
			{
				outModalSessionAborted = true;
				break;
			}
		}
		
		AIndex	textpos = spos;
		mRegExp.InitGroup();
		if( mRegExp.Match(inText,textpos,inEpos) )//B0050
		{
			outSpos = spos;//B0192
			outEpos = textpos;//B0192
			result = true;
			break;
		}
		
		//#789
		if( inReverse == false )
		{
			//順方向検索の場合、次の文字へ移動
			spos = inText.GetNextCharPos(spos);
		}
		else
		{
			//逆方向検索の場合、前の文字へ移動
			spos = inText.GetPrevCharPos(spos,true);
		}
		
		//sposがinTextの範囲外になったら終了
		if( spos >= inText.GetItemCount() )   break;
		if( spos < 0 )   break;
	}
	return result;
}

void	ATextFinder::ChangeReplaceText( const AText& inTargetText, const AText& inReplaceText, AText& outText ) const
{
	mRegExp.ChangeReplaceText(inTargetText,inReplaceText,outText);
}

//#166
/**
無視文字判定
*/
ABool	ATextFinder::IsIgnoreChar( const AUCS4Char inUCS4Char, const ABool isIgnoreSpaces, const ABool isIgnoreOnbiki ) const
{
	ABool	result = false;
	//スペース文字
	if( isIgnoreSpaces == true )
	{
		switch(inUCS4Char)
		{
		  case 0x0020://space
		  case 0x0009://tab
		  case 0x000D://return
		  case 0x3000://全角space
		  case 0x00A0://no-break-space (normal)
		  case 0x202F://no-break-space (narrow)
		  case 0x2060://no-break-space (zero width)
			{
				result = true;
				break;
			}
		}
	}
	//音引き
	if( isIgnoreOnbiki == true )
	{
		switch(inUCS4Char)
		{
		  case 0x30FC://全角音引き
		  case 0xFF70://半角音引き
			{
				result = true;
				break;
			}
		}
	}
	return result;
}

void	AFindParameter::Set( const AFindParameter& inSrc )
{
	findText.					SetText(inSrc.findText);
	replaceText.				SetText(inSrc.replaceText);
	ignoreCase 					= inSrc.ignoreCase;
	wholeWord 					= inSrc.wholeWord;
	fuzzy 						= inSrc.fuzzy;
	regExp 						= inSrc.regExp;
	loop 						= inSrc.loop;
	ignoreBackslashYen 			= inSrc.ignoreBackslashYen;
	filterText.					SetText(inSrc.filterText);
	recursive 					= inSrc.recursive;
	displayPosition 			= inSrc.displayPosition;
	folderPath.					SetText(inSrc.folderPath);
	//modeIndex 					= inSrc.modeIndex;
	ignoreSpaces				= inSrc.ignoreSpaces;//#165
	//B0313
	onlyVisibleFile				= inSrc.onlyVisibleFile;
	onlyTextFile				= inSrc.onlyTextFile;
}

