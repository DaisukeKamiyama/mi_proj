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

ARegExp
正規表現検索／置換

*/

#include "stdafx.h"

#include "ARegExp.h"

/*  

RegExp中間表現

制御文字00H
00 00

グループスタート
00 01 xx   xx:グループ番号
グループ終了
00 02 xx   xx:グループ番号

繰り返し
00 10 nn mm ss ll ll   nn:n mm:m ss:1=最長 0=最短 llll:length 
バージョン2.1b8d6より下記の形式に変更
00 10 nn nn mm mm ss ll ll   nn nn:n mm mm:m ss:1=最長 0=最短 llll:length 

範囲
00 20 xx   xx:範囲参照番号

選択
00 30 xx nn ll1 ll1 ll2 ll2 ... lln lln  xx:選択番号　nn:選択数　llllx:length
各選択
... 00 31 jj jj   llll:length jjjj:jump

後方参照
00 40 xx   xx:グループ番号

任意の１文字(\r以外)
00 50

行頭
00 60
行末
00 61

単語区切り(\b)
00 62
単語区切り以外(\B)
00 63

後続指定（マッチ）
00 70
後続指定（マッチしない）
00 71

例 (abc)+
00 10 00 01 06		00 01 00		abc		00 02 00

*/

/*

URegExpの使い方

まず、URegExp()でインスタンスを作成します。
SetRE()で、正規表現をセットします。
Match(textポインタ,text長さ,開始位置)で、検索を行います
mGroupRef[](CText)にグループの一致文字列が入っています
再度同じ検索を行う時は、InitGroup()で グループをクリアして下さい。

*/ 

ARegExp::ARegExp( AObjectArrayItem* inParent ) : AObjectArrayItem(inParent), mIgnoreCase(false)//R0233
{
	Init();
}
//#1231
ARegExp::ARegExp( AConstCharPtr inRE, const ABool inIgnoreCase, const ABool inHandleYenAsBackslash ): AObjectArrayItem(NULL), mIgnoreCase(false)
{
	Init();
	AText	re(inRE);
	SetRE(re,inIgnoreCase,inHandleYenAsBackslash);
}

void	ARegExp::InitGroup()
{
	mMatchedTextSpos = 0;
	mMatchedTextEpos = 0;
	//#1230
	for( short i = 0; i < kGroupCountMax; i++ )
	{
		mGroupSpos[i] = 0;
		mGroupEpos[i] = 0;
	}
}	

void	ARegExp::Init()
{
	mIex.DeleteAll();
	for( short i = 0; i < kRangeCountMax; i ++ ) 
	{
		mRange[i].Init();
	}
	mRangeNumber = 0;
	mGroupCount = 0;
	InitGroup();
	mREValid = false;//B0317
}

ABool	ARegExp::SetRE( const AText& inRE, const ABool inIgnoreCase, const ABool inHandleYenAsBackslash )//B0029 B0317 R0233 #501
{
	//R0233
	mIgnoreCase = inIgnoreCase;
	//B0317 \を\に変換→やっぱやめ
	AText	re;
	re.SetText(inRE);
	/*AText	yen("\");
	AText	backslash("\\");
	re.ReplaceText(yen,backslash);*/
	//#501 半角￥からバックスラッシュへの変換処理
	if( inHandleYenAsBackslash == true )
	{
		for( AIndex p = 0; p < re.GetItemCount(); p = re.GetNextCharPos(p) )
		{
			//文字取得
			AUChar	ch1 = 0, ch2 = 0;
			ch1 = re.Get(p);
			if( p+1 < re.GetItemCount() )
			{
				ch2 = re.Get(p+1);
			}
			//半角￥なら変換処理
			if( ch1 == 0xC2 && ch2 == 0xA5 )
			{
				//文字変換￥→バックスラッシュ
				re.Delete1(p);
				re.Set(p,0x5C);
				//￥の次の文字は変換対象外とする（￥なら￥のままにすべき）
				p++;
			}
			//バックスラッシュの次の文字は変換対象外
			if( ch1 == 0x5C )
			{
				p++;
			}
		}
	}
	//
	Init();
	AIndex	groupnum = 0;
	//B0317 throwされなかった場合のみmREValidをtrueにする。mREValidがfalseの場合はMatch()を必ずfalseにする
	try
	{
		if( re.GetItemCount() > 0 )
		{
			Parse(re,0,re.GetItemCount(),groupnum);
			mREValid = true;
		}
	}
	catch(...)
	{
	}
	mGroupCount = groupnum;
	mRE.SetText(re);
	return mREValid;
}

void ARegExp::GetRE( AText& outRE ) const
{
	outRE.SetText(mRE);
}

void	ARegExp::Parse( const AText& inRE, const AIndex inStart, AIndex inEnd, AIndex &ioGroupNumber )
{
	if( inEnd > inRE.GetItemCount() )   inEnd = inRE.GetItemCount();
	/*選択
	00 30 nn   nn:選択数
	各選択
	ll ll ... 00 31 jj jj   llll:length jjjj:jump position*/
	AArray<AIndex>	selectSpos;
	selectSpos.Add(inStart);
	short	glevel = 0;
	short	rlevel = 0;
	for( AIndex REPos = inStart; REPos < inEnd; REPos++ ) 
	{
		AUChar ch = inRE.Get(REPos);
		if( IsUTF8Multibyte(ch) )
		{
			REPos += inRE.GetUTF8ByteCount(ch)-1/*B0218*/;
			continue;
		}
		switch(ch) 
		{
		  case '(':
			glevel++;
			break;
		  case ')':
			glevel--;
			break;
		  case '[':
			rlevel++;
			break;
		  case ']':
			rlevel--;
			break;
		  case '\\':
			REPos++;
			break;
		  case '|':
			{
				if( glevel == 0 && rlevel == 0 ) 
				{
					selectSpos.Add(REPos+1);
				}
				break;
			}
		}
	}
	AItemCount	selectCount = selectSpos.GetItemCount();
	selectSpos.Add(inEnd+1);
	if( selectCount != 1 ) 
	{
		mIex.Add(0);
		mIex.Add(0x30);
		mIex.Add(selectCount);
		AArray<AIndex>	jumpPos;
		for( short i = 0; i < selectCount; i++ ) 
		{
			mIex.Add(0);//Length
			mIex.Add(0);
			AIndex	svPos = mIex.GetItemCount();
			Parse(inRE,selectSpos.Get(i),selectSpos.Get(i+1)-1,ioGroupNumber);
			mIex.Add(0);
			mIex.Add(0x31);
			jumpPos.Add(mIex.GetItemCount());
			mIex.Add(0);//jump
			mIex.Add(0);
			unsigned short	len = mIex.GetItemCount()-svPos;
			mIex.Set(svPos-2,(len>>8)&0xFF);
			mIex.Set(svPos-1,(len)&0xFF);
		}
		for( short i = 0; i < jumpPos.GetItemCount(); i++ ) 
		{
			AIndex	pos = jumpPos.Get(i);
			unsigned short	jump = mIex.GetItemCount();
			mIex.Set(pos,(jump>>8)&0xFF);
			mIex.Set(pos+1,(jump)&0xFF);
		}
		return;
	}
	
	for( AIndex	REPos = inStart; REPos < inEnd;  ) {
		AUChar	ch = inRE.Get(REPos);
		switch(ch) 
		{
		  case '^'://行頭
			{
				REPos++;
				mIex.Add(0);
				mIex.Add(0x60);
				break;
			}
		  case '$'://行末
			{
				REPos++;
				mIex.Add(0);
				mIex.Add(0x61);
				break;
			}
		  case '('://Group
			{
				ABool	nogroupmode = false;
				ABool	aftermode = false;
				ABool	afternotmode = false;
				REPos++;
				if( REPos >= inEnd )   throw 0;//B0317
				if( inRE.Get(REPos) == '?' ) 
				{
					if( REPos+1 >= inRE.GetItemCount() )   throw 0;//B0000
					if( inRE.Get(REPos+1) == '#' ) {//コメント
						for( REPos += 2; REPos < inEnd; REPos++ ) 
						{
							if( inRE.Get(REPos) == ')' ) 
							{
								REPos++;
								break;
							}
						}
						break;
					}
					switch( inRE.Get(REPos+1) ) 
					{
					  case ':':
						REPos += 2;
						nogroupmode = true;
						break;
					  case '=':
						REPos += 2;
						nogroupmode = true;
						aftermode = true;
						break;
					  case '!':
						REPos += 2;
						nogroupmode = true;
						afternotmode = true;
						break;
					}
				}
				AIndex	groupstart = REPos;//(のうしろ
				short	level = 1;
				for( ; REPos < inEnd; REPos++ ) 
				{
					if( inRE.Get(REPos-1) != '\\' && inRE.Get(REPos) == '(' ) 
					{
						level++;
					}
					else if(  inRE.Get(REPos-1) != '\\' && inRE.Get(REPos) == ')' ) 
					{
						level--;
					}
					if( level == 0 ) 
					{
						break;
					}
				}
				if( REPos >= inEnd ) 
				{
					throw 0;//)がない
				}
				AIndex	groupend = REPos;//)の位置
				REPos++;//)のうしろ
				
				ABool	repeatMode = false;
				AIndex	repeatSpos;
				if( !aftermode && !afternotmode ) 
				{
					if( ParseRepMeta(inRE,REPos,0) ) 
					{//グループ繰り返し
						repeatSpos = mIex.GetItemCount();
						repeatMode = true;
					}
				}
				else if( aftermode ) 
				{
					mIex.Add(0);
					mIex.Add(0x70);
				}
				else if( afternotmode ) {
					mIex.Add(0);
					mIex.Add(0x71);
				}
				
				AIndex	groupNumber = ioGroupNumber;
				if( groupNumber < kGroupCountMax && !nogroupmode ) 
				{
					mIex.Add(0);
					mIex.Add(1);//グループスタート
					mIex.Add(groupNumber);
					ioGroupNumber++;
				}
				
				Parse(inRE,groupstart,groupend,ioGroupNumber);
				
				if( repeatMode ) 
				{
					unsigned short	len = mIex.GetItemCount()-repeatSpos;
					mIex.Set(repeatSpos-2,(len>>8)&0xFF);
					mIex.Set(repeatSpos-1,(len)&0xFF);
				}
				
				if( groupNumber < kGroupCountMax && !nogroupmode) 
				{
					mIex.Add(0);
					mIex.Add(2);//グループ終了
					mIex.Add(groupNumber);
				}
				break;
			}
		  case '[':
			{
				if( mRangeNumber > 99 )   throw 0;
				REPos++;
				ABool	notmode = false;
				if( REPos >= inRE.GetItemCount() )   throw 0;//B0000
				ch = inRE.Get(REPos);
				if( ch == '^' ) 
				{
					notmode = true;
					REPos++;
				}
				if( REPos >= inRE.GetItemCount() )   throw 0;//B0000
				ch = inRE.Get(REPos);
				if( ch == ']' ) 
				{//最初の]は範囲文字とする
					mRange[mRangeNumber].AddRange(ch,ch);
					REPos++;
				}
				mRange[mRangeNumber].SetNotMode(notmode);
				ABool	readonemode = false;
				ABool	rangemode = false;
				/*#731 AUTF16Char*/AUCS4Char	start,end;
				for( ; REPos <= inEnd; ) 
				{
					if( REPos >= inRE.GetItemCount() )   throw 0;//B0000
					AUChar ch = inRE.Get(REPos);
					REPos++;
					if( readonemode ) 
					{//状態2：１文字すでに読み込んだ状態
						if( ch == '-' ) 
						{
							rangemode = true;
						}
						else 
						{
							mRange[mRangeNumber].AddRange(start,start);
							if( ch == ']' )   break;
							REPos--;
						}
						readonemode = false;
					}
					else if( rangemode ) 
					{//状態3：'-'を読み込んだ状態
						if( ch == ']' )   throw 0;
						if( ch == '\\' ) 
						{
							if( REPos >= inRE.GetItemCount() )   throw 0;//B0000
							ch = inRE.Get(REPos);
							if( !ParseEscape(inRE,REPos,end) )   throw 0;
						}
						else 
						{
							if( inRE.IsUTF8MultiByte(ch) )
							{
								REPos--;
								REPos += inRE./*#731Convert1CharToUTF16*/Convert1CharToUCS4(REPos,end);
							}
							else 
							{
								end =  ch;
							}
						}
						mRange[mRangeNumber].AddRange(start,end);
						rangemode = false;
					}
					else 
					{//状態1：何も読み込んでいない状態
						ABool	abbrev = false;
						if( ch == ']' )   break;
						if( ch == '\\' ) 
						{
							if( REPos >= inRE.GetItemCount() )   throw 0;//B0000
							ch = inRE.Get(REPos);
							switch(ch) 
							{
							  case 'w':
								mRange[mRangeNumber].AddRange('0','9');
								mRange[mRangeNumber].AddRange('A','Z');
								mRange[mRangeNumber].AddRange('a','z');
								mRange[mRangeNumber].AddRange('_','_');
								abbrev = true;
								REPos++;
								break;
							  case 'd':
								mRange[mRangeNumber].AddRange('0','9');
								abbrev = true;
								REPos++;
								break;
							  case 's':
								mRange[mRangeNumber].AddRange(0x20,0x20);
								mRange[mRangeNumber].AddRange(0xa,0xa);
								mRange[mRangeNumber].AddRange(0xc,0xc);
								mRange[mRangeNumber].AddRange(0xd,0xd);
								mRange[mRangeNumber].AddRange(0x9,0x9);
								abbrev = true;
								REPos++;
								break;
								//#1212 \x(\u)で範囲指定ができない問題を修正
							  case 'u':
							  case 'x':
								{
									AUCS4Char	letter = 0x2000;
									if( !ParseEscape(inRE,REPos,letter) )   throw 0;
									start = letter;
									abbrev = true;
									readonemode = true;
									break;
								}
								//
							  default:
								/*#731 AUTF16Char*/AUCS4Char	letter;
								if( !ParseEscape(inRE,REPos,letter) )   throw 0;
								mRange[mRangeNumber].AddRange(letter,letter);
								abbrev = true;
								break;
							}
						}
						if( !abbrev ) 
						{
							if( inRE.IsUTF8MultiByte(ch) )
							{
								REPos--;
								REPos += inRE./*#731 Convert1CharToUTF16*/Convert1CharToUCS4(REPos,start);
							}
							else 
							{
								start =  ch;
							}
							readonemode = true;
						}
					}
				}
				ParseRepMeta(inRE,REPos,3);//範囲繰り返し
				mIex.Add(0);
				mIex.Add(0x20);
				mIex.Add(mRangeNumber);
				mRangeNumber++;
				break;
			}
		  case '\\':
			{
				REPos++;
				AIndex	groupref;
				if( ParseInteger(inRE,REPos,groupref) ) 
				{//後方参照
					if( groupref == 0 ) 
					{//制御文字00H
						ParseRepMeta(inRE,REPos,2);
						mIex.Add(0);
						mIex.Add(0);
					}
					else 
					{
						groupref--;
						if( groupref < kGroupCountMax ) 
						{
							ParseRepMeta(inRE,REPos,3);
							mIex.Add(0);
							mIex.Add(0x40);
							mIex.Add(groupref);
						}
					}
				}
				else 
				{//エスケープ文字
					if( REPos >= inRE.GetItemCount() )   throw 0;//B0000
					ch = inRE.Get(REPos);
					switch(ch) 
					{
					  case 'w':
					  case 'W':
					  case 'd':
					  case 'D':
					  case 's':
					  case 'S':
						{
							REPos++;
							switch(ch) 
							{
							  case 'w':
							  case 'W':
								mRange[mRangeNumber].AddRange('0','9');
								mRange[mRangeNumber].AddRange('A','Z');
								mRange[mRangeNumber].AddRange('a','z');
								mRange[mRangeNumber].AddRange('_','_');
								break;
							  case 'd':
							  case 'D':
								mRange[mRangeNumber].AddRange('0','9');
								break;
							  case 's':
							  case 'S':
								mRange[mRangeNumber].AddRange(0x20,0x20);
								mRange[mRangeNumber].AddRange(0xa,0xa);
								mRange[mRangeNumber].AddRange(0xc,0xc);
								mRange[mRangeNumber].AddRange(0xd,0xd);
								mRange[mRangeNumber].AddRange(0x9,0x9);
								break;
							}
							if( ch == 'W' || ch == 'D' || ch == 'S' ) 
							{
								mRange[mRangeNumber].SetNotMode(true);
							}
							ParseRepMeta(inRE,REPos,3);
							mIex.Add(0);
							mIex.Add(0x20);
							mIex.Add(mRangeNumber);
							mRangeNumber++;
							break;
						}
					  case 'b':
						{
							REPos++;
							mIex.Add(0);
							mIex.Add(0x62);
							break;
						}
					  case 'B':
						{
							REPos++;
							mIex.Add(0);
							mIex.Add(0x63);
							break;
						}
					  default:
						{
							/*#731 AUTF16Char*/AUCS4Char	letter;
							if( !ParseEscape(inRE,REPos,letter) ) 
							{
								throw 0;
							}
							//#1214 mIexにUTF16で指定してしまっていた問題を修正。
							AText	utf8text;
							utf8text.AddFromUCS4Char(letter);
							for( AIndex i = 0; i < utf8text.GetItemCount(); i++ )
							{
								mIex.Add(utf8text.Get(i));
							}
							/*#1204 mIexにはUTF8で設定すべきなので、上の処理へ修正
							if( letter < 0x100 ) 
							{
								ParseRepMeta(inRE,REPos,1);
								mIex.Add(letter);
							}
							else {
								ParseRepMeta(inRE,REPos,2);
								mIex.Add((letter>>8)&0xFF);
								mIex.Add((letter)&0xFF);
							}
							*/
							break;
						}
					}
				}
				break;
			}
		  case '.':
			{
				REPos++;
				ParseRepMeta(inRE,REPos,2);
				mIex.Add(0);
				mIex.Add(0x50);
				break;
			}
		  default:
			{
				AIndex	svREPos = REPos;
				AItemCount	utf8len = inRE.GetUTF8ByteCount(ch);
				REPos += utf8len;
				ParseRepMeta(inRE,REPos,utf8len);
				for( AIndex j = 0; j < utf8len; j++ )
				{
					ch = inRE.Get(svREPos+j);
					//R0233 IgnoreCaseの場合は全て小文字にする
					if( mIgnoreCase == true )
					{
						if( ch >= 'A' && ch <= 'Z' )
						{
							ch += 'a'-'A';
						}
					}
					
					mIex.Add(ch);
				}
				break;
			}
		}
	}
}

ABool	ARegExp::ParseEscape( const AText& inRE, AIndex &ioREPos, /*#731 AUTF16Char*/AUCS4Char &outLetter )
{
	AIndex	REPos = ioREPos;
	AUChar ch = inRE.Get(REPos);
	REPos++;
	switch(ch) 
	{
	  case 'x'://16進数
	  case 'u'://unicode #1204
		{
			AUCS4Char	num = 0x2000;//win #1214 AUInt16→AUCS4Char
			if( !ParseHex(inRE,REPos,num) ) //win
			{
				return false;
			}
			outLetter = num;//win
			break;
		}
	  case 'c':
		{
			if( REPos >= inRE.GetItemCount() )   throw 0;//B0000
			ch = inRE.Get(REPos);
			REPos++;
			if( ch >= 'a' && ch <= 'z' ) 
			{
				outLetter = ch - 'a' +1;
			}
			else 
			{
				return false;
			}
			break;
		}
	  case 't':
		outLetter = '\t';
		break;
	  case 'r':
	  case 'n'://#938
		outLetter = '\r';
		break;
	  case 'f':
		outLetter = 0xc;
		break;
	  case 'b':
		outLetter = 0x8;
		break;
	  case 'a':
		outLetter = 0x7;
		break;
		/*#938
	  case 'n':
		outLetter = 0xa;
		break;
		*/
	  case 'e':
		outLetter = 0x1b;
		break;
	  case '0':
		outLetter = 0;
		break;
	  /*case '\'':
		outLetter = '\'';
		break;
	  case '\"':
		outLetter = '\"';
		break;
	  case '$':
		outLetter = '$';
		break;
	  case '@':
		outLetter = '@';
		break;
	  case '\\':
		outLetter = '\\';
		break;
	  case '[':
		outLetter = '[';
		break;
	  case ']':
		outLetter = ']';
		break;
	  case '-':
		outLetter = '-';
		break;
	  case '(':
		outLetter = '(';
		break;
	  case ')':
		outLetter = ')';
		break;*/
	  default:
		outLetter = ch;
		break;
	}
	ioREPos = REPos;
	return true;
}

ABool	ARegExp::ParseInteger( const AText& inRE, AIndex &ioREPos, ANumber &outInt ) const
{
	outInt = 0;
	ABool	result = false;
	for( short i = 0; i <= 5; i++ ) 
	{
		if( ioREPos >= inRE.GetItemCount() )   break;
		AUChar ch = inRE.Get(ioREPos);
		if( !(ch >= '0' && ch <= '9') ) 
		{
			break;
		}
		outInt *= 10;
		outInt += ch-'0';
		ioREPos++;
		result = true;
	}
	return result;
}

ABool	ARegExp::ParseHex( const AText& inRE, AIndex &ioREPos, AUCS4Char &outHex )// #1214 AUInt16→AUCS4Char
{
	outHex = 0;
	ABool	result = false;
	for( short i = 0; i < /*#1204 4*/16; i++ ) //#1204 4→16 \x{}のカッコ、および、HEX桁数増加のため。無限ループ防止カウンタなので、適度に増やしておく。
	{
		if( ioREPos >= inRE.GetItemCount() )   break;
		AUChar ch = inRE.Get(ioREPos);
		//#1204 \x{}対応
		//{なら無視して次の文字へ
		if( ch == '{' )
		{
			ioREPos++;
			continue;
		}
		//}なら次の文字へ移動して終了
		if( ch == '}' )
		{
			ioREPos++;
			break;
		}
		//
		if( !((ch >= '0' && ch <= '9')||(ch >= 'A' && ch <= 'Z')||(ch >= 'a' && ch <= 'z')) ) 
		{
			break;
		}
		outHex *= 16;
		if( ch >= '0' && ch <= '9' ) 
		{
			outHex += ch-'0';
		}
		if( ch >= 'a' && ch <= 'z' ) 
		{
			outHex += ch - 'a'+10;
		}
		if( ch >= 'A' && ch <= 'Z' ) 
		{
			outHex += ch - 'A'+10;
		}
		ioREPos++;
		result = true;
	}
	return result;
}

ABool	ARegExp::ParseRepMeta( const AText& inRE, AIndex &ioREPos, const AItemCount inLength )
{
	if( ioREPos >= inRE.GetItemCount() )   return false;
	AIndex	REPos = ioREPos;
	ABool	saicho = true;
	ANumber	n,m;
	AUChar ch1 = ch1 = inRE.Get(REPos);
	AUChar	ch2 = 0;
	if( REPos+1 < inRE.GetItemCount() )
	{
		ch2 = inRE.Get(REPos+1);
	}
	switch(ch1) 
	{
	  case '*':
		{
			n = 0;
			m = 0xFFFF;
			REPos++;
			if( REPos < inRE.GetItemCount() && ch2 == '?' ) {
				saicho = false;
				REPos++;
			}
			ioREPos = REPos;
			break;
		}
	  case '+':
		{
			n = 1;
			m = 0xFFFF;
			REPos++;
			if( REPos < inRE.GetItemCount() && ch2 == '?' ) {
				saicho = false;
				REPos++;
			}
			ioREPos = REPos;
			break;
		}
	  case '?':
		{
			n = 0;
			m = 1;
			REPos++;
			if( REPos < inRE.GetItemCount() && ch2 == '?' ) {
				saicho = false;
				REPos++;
			}
			ioREPos = REPos;
			break;
		}
	  case '{':
		{
			REPos++;
			if( !ParseInteger(inRE,REPos,n) ) {
				return false;
			}
			if( REPos >= inRE.GetItemCount() )   return false;
			if( inRE.Get(REPos) != ',' ) 
			{
				return false;
			}
			REPos++;
			if( !ParseInteger(inRE,REPos,m) ) 
			{
				m = 0xFFFF;
			}
			if( REPos >= inRE.GetItemCount() )   return false;
			if( inRE.Get(REPos) != '}' )
			{
				return false;
			}
			REPos++;
			if( REPos < inRE.GetItemCount() && inRE.Get(REPos) == '?' ) 
			{
				saicho = false;
				REPos++;
			}
			ioREPos = REPos;
			if( n > 0xFFFE )   n = 0xFFFE;
			if( m > 0xFFFE )   m = 0xFFFE;
			if( n > m )   throw 0;
			break;
		}
		default:
		return false;
	}
	mIex.Add(0);
	mIex.Add(0x10);//繰り返し
	AUChar	ch;
	ch = (n&0xFF00)/0x100;
	mIex.Add(ch);
	ch = (n&0xFF);
	mIex.Add(ch);
	ch = (m&0xFF00)/0x100;
	mIex.Add(ch);
	ch = (m&0xFF);
	mIex.Add(ch);
	mIex.Add((saicho?1:0));
	mIex.Add(0);
	mIex.Add(inLength);
	return true;
}

//一致判定
// inText: 一致判定を行う対象テキスト
// ioTextPos: 一致判定を行うテキスト位置
// inTargetEndPos: 一致判定対象範囲の終了位置
ABool	ARegExp::Match( const AText& inText, AIndex &ioTextPos, const AIndex inTextTargetEndPos )//B0050 inTotalLength追加
{
	if( mREValid == false )   return false;//B0317
	mCanceled = false;
	ABool result = false;
	try
	{
		mDepth = 0;
		mLastMatchedGroup = -1;
		mEndPosition = -1;
		AIndex	spos = ioTextPos;
		AIndex	iexPos = 0;
		{//#598 arrayptrの有効範囲を最小にする
			//Iexのポインタ取得
			AStTextPtr	iexptr(mIex,0,mIex.GetItemCount());
			//inTextのポインタ取得 （#731 高速化のためポインタ使用に変更）
			AStTextPtr	textptr(inText,0,inText.GetItemCount());
			//検索実行
			result =  MatchRecursive(iexptr.GetPtr(),iexptr.GetByteCount(),iexPos,
						//#731 inText,
						textptr.GetPtr(),textptr.GetByteCount(),//#731
						inTextTargetEndPos,ioTextPos);//B0050
		}
		if( mEndPosition != -1 )   ioTextPos = mEndPosition;
		else mEndPosition = ioTextPos;
		mMatchedTextSpos = spos;
		mMatchedTextEpos = ioTextPos;
	}
	catch(...)
	{
	}
	return result;
}
//#1231
ABool	ARegExp::Match( const AText& inText )
{
	AIndex	pos = 0;
	return Match(inText,pos,inText.GetItemCount());
}

ABool	ARegExp::MatchRecursive( const AConstUCharPtr iex, const AItemCount inIexLength, AIndex &ioIexPos, //#731 const AText& inText, 
		const AConstUCharPtr inTextPtr, const AItemCount inTextLength,//#731
		const AIndex inTextTargetEndPos, AIndex &ioTextPos )//B0050 inTotalLength追加
{
	ADepth	depthinc(mDepth);
	if( mDepth > 256 )   {_ACError("",this); return false;}//再帰が深くなり過ぎるのを防ぐ
	
	for( ; ioIexPos < inIexLength; ) 
	{
		AUChar	ch = iex[ioIexPos];
		if( ch == 0 ) 
		{
			ioIexPos++;
			ch = iex[ioIexPos];
			ioIexPos++;
			switch(ch) 
			{
			  case 0://制御文字00H
				{
					break;
				}
			  case 1://グループスタート
				{
					ch = iex[ioIexPos];
					ioIexPos++;
					mGroupSpos[ch] = ioTextPos;
					break;
				}
			  case 2://グループ終了
				{
					ch = iex[ioIexPos];
					ioIexPos++;
					mGroupEpos[ch] = ioTextPos;
					if( ch > mLastMatchedGroup )   mLastMatchedGroup = ch;
					break;
				}
			  case 0x10://繰り返し 00 10 nn nn mm mm ss ll   nn nn:n mm mm:m ss:1=最長 0=最短 ll:length ※バージョン2.1b8d6より繰り返し数を１バイトから２バイトにしました
				{
					ANumber	n, m;
					ABool	saicho;
					n = iex[ioIexPos]*0x100 + iex[ioIexPos+1];
					ioIexPos += 2;
					m = iex[ioIexPos]*0x100 + iex[ioIexPos+1];
					ioIexPos += 2;
					ch = iex[ioIexPos];
					ioIexPos++;
					saicho = (ch == 1);
					AItemCount	len = 0;
					len = iex[ioIexPos];
					ioIexPos++;
					len *= 0x100;
					len += iex[ioIexPos];
					ioIexPos++;
					AIndex repeatStart = ioIexPos;
					AItemCount repeatLength = len;
					if(saicho) 
					{//最長一致
						AArray<AIndex>	longArray;
						//Int32	endPos[256];
						//Int32	endPosCount = 0;
						longArray.DeleteAll();//はじめにendPosに各繰り返しの終了位置を保存していく
						AIndex	textPos = ioTextPos;
						//endPos[endPosCount] = textPos;
						//endPosCount++;
						longArray.Add(textPos);//０回繰り返し
						//繰り返し部分だけの正規表現でマッチを繰り返し行い、一致しなくなったらループを抜ける
						for( ANumber rep = 1; (rep <= m || m == 0xFFFF) ; rep++ ) 
						{
							AIndex	iexPos = 0;
							if( !MatchRecursive(iex+repeatStart,repeatLength,iexPos,//#731 inText,
									inTextPtr,inTextLength,inTextTargetEndPos,textPos) )//B0050 
							{
								break;
							}
							//B0397 0文字一致の繰り返しで無限ループになってしまう問題対策
							//longArrayに格納した最後のtext位置が、今回の一致結果textPosと一緒なら、ここで終了
							//（これ以上は何度繰り返しても、textPosが進まない）
							if( longArray.Get(longArray.GetItemCount()-1) == textPos )   break;
							
							//endPos[endPosCount] = textPos;
							//endPosCount++;
							longArray.Add(textPos);
						}
						if( /*endPosCount-1*/longArray.GetItemCount()-1 < n )   return false;
						//上記ループの最長繰り返しの終了位置から繰り返し部分以降のマッチを行い、
						//一致しなければどんどん短い繰り返しを試していく
						for( ANumber	rep = longArray.GetItemCount()-1/*endPosCount-1*/; rep >= n; rep-- ) 
						{
							AIndex	textPos = ioTextPos;
							if( rep > 1 ) 
							{
								textPos = longArray.Get(rep-1);/*endPos[rep-1];*/
							}
							if( rep > 0 ) 
							{
								AIndex	iexPos = ioIexPos;
								if( MatchRecursive(iex,inIexLength,iexPos,//#731 inText,
									inTextPtr,inTextLength,inTextTargetEndPos,textPos) ) //B0050
								{
									ioIexPos = iexPos;
									ioTextPos = textPos;
									return true;
								}
							}
							else 
							{
								ioIexPos += len;
								return MatchRecursive(iex,inIexLength,ioIexPos,//#731 inText,
									inTextPtr,inTextLength,inTextTargetEndPos,ioTextPos);//B0050
							}
						}
					}
					else 
					{//最短一致
						AIndex	textPos = ioTextPos;
						AIndex	iexPos = ioIexPos;
						if( n == 0 ) 
						{
							AIndex	iexPosZero = iexPos+len;
							if( MatchRecursive(iex,inIexLength,iexPosZero,//#731 inText,
									inTextPtr,inTextLength,inTextTargetEndPos,textPos) )//B0050 
							{
								ioIexPos = iexPosZero;
								ioTextPos = textPos;
								return true;
							}
						}
						textPos = ioTextPos;
						for( ANumber rep = 1; (rep <= m || m == 0xFFFF); rep++ ) 
						{
							AIndex	repiexPos = 0;
							AIndex	reptextPos = textPos;
							if( !MatchRecursive(iex+repeatStart,repeatLength,repiexPos,//#731 inText,
									inTextPtr,inTextLength,inTextTargetEndPos,reptextPos) )//B0050 
							{
								return false;
							}
							if( rep >= n ) 
							{
								AIndex	aftertextPos = textPos;
								iexPos = ioIexPos;
								if( MatchRecursive(iex,inIexLength,iexPos,//#731 inText,
									inTextPtr,inTextLength,inTextTargetEndPos,aftertextPos) ) //B0050
								{
									ioIexPos = iexPos;
									ioTextPos = aftertextPos;
									return true;
								}
							}
							//B0397 0文字一致の繰り返しで無限ループになってしまう問題対策
							//reptextPosがtextPosと一緒なら、ここで終了
							//（これ以上は何度繰り返しても、textPosが進まない）
							if( textPos == reptextPos )   return false;
							
							textPos = reptextPos;
						}
					}
					break;
				}
			  case 0x20://範囲
				{
					AIndex	rangenum = iex[ioIexPos];
					ioIexPos++;
					if( ioTextPos >= inTextTargetEndPos )   return false;
					//R-uni
					/*#731 AUTF16Char*/AUCS4Char	letter;
					ioTextPos += /*#731 inText.Convert1CharToUTF16*/AText::Convert1CharToUCS4(inTextPtr,inTextLength,ioTextPos,letter);
					
					if( !mRange[rangenum].Match(letter) )   return false;
					break;
				}
			  case 0x30://選択
				{
					/*選択
					00 30 nn nn:選択数
					各選択
					ll ll ... 00 31 jj jj   llll:length jjjj:jump position*/
					AItemCount	selectCount = iex[ioIexPos];
					ioIexPos++;
					for( short i = 0; i < selectCount; i++ ) {
						unsigned short	len = iex[ioIexPos]*0x100+iex[ioIexPos+1];
						ioIexPos += 2;
						AIndex	iexPos = ioIexPos;
						AIndex	textPos = ioTextPos;
						if( MatchRecursive(iex,inIexLength,iexPos,//#731 inText,
									inTextPtr,inTextLength,inTextTargetEndPos,textPos) ) {//B0050
							ioIexPos = iexPos;
							ioTextPos = textPos;
							return true;
						}
						ioIexPos += len;
					}
					return false;
					break;
				}
			  case 0x31://選択終了（絶対ジャンプ）
				{
					AIndex	pos = iex[ioIexPos]*0x100+iex[ioIexPos+1];
					ioIexPos = pos;
					break;
				}
			  case 0x40://後方参照
				{
					AIndex	groupnum = iex[ioIexPos];
					ioIexPos++;
					if( groupnum < kGroupCountMax ) 
					{
						AText	grouptext;
						//#731 inText.GetText(mGroupSpos[groupnum],mGroupEpos[groupnum]-mGroupSpos[groupnum],grouptext);
						if( mGroupEpos[groupnum] > mGroupSpos[groupnum] )//#1230 正規表現(A)|(B)で一致して$1$2を置換する場合など、$1のsposのみ設定されてeposが設定されていないことがあるので、その場合は空文字を追加する。
						{
							grouptext.SetFromTextPtr(&(inTextPtr[mGroupSpos[groupnum]]),mGroupEpos[groupnum]-mGroupSpos[groupnum]);//#731
						}
						for( AIndex i = 0; i < grouptext.GetItemCount(); i++ ) 
						{
							if( ioTextPos >= inTextTargetEndPos )   return false;
							if( grouptext.Get(i) != inTextPtr[ioTextPos] )//#731 inText.Get(ioTextPos) ) 
							{
								return false;
							}
							ioTextPos++;
						}
					}
					break;
				}
			  case 0x50://任意の１文字（\r以外）
				{
					if( ioTextPos >= inTextTargetEndPos )   return false;
					/*#731 AUTF16Char*/AUCS4Char	letter;
					ioTextPos += /*#731 inText.Convert1CharToUTF16*/AText::Convert1CharToUCS4(inTextPtr,inTextLength,ioTextPos,letter);
					if( letter == 13 )   return false;
					break;
				}
			  case 0x60://行頭
				{
					if( ioTextPos > 0 ) 
					{
						if( /*#731inText.Get(ioTextPos-1)*/inTextPtr[ioTextPos-1] != '\r' )   return false;
						//B0056 一致したのが選択範囲の最後の場合は不一致と見なす
						//ただし、テキストの最後だけは一致。（選択範囲内置換以外の場合で、最終行が空行だった場合）→B0440 この条件は不要。
						if( ioTextPos == inTextTargetEndPos /*B0440 && ioTextPos != inText.GetItemCount()*/ )
						{
							return false;
						}
					}
					break;
				}
			  case 0x61://行末
				{
					//B0050 選択範囲でなくテキスト領域全体で行末かどうかを判定すべき
					//B0047 -1は不要。（テキスト最後から１文字前が一致してしまう）
					if( ioTextPos < inTextLength) //#731 *inTextLength*/inText.GetItemCount()/*-1*/ ) 
					{
						if( /*#731 inText.Get(ioTextPos)*/inTextPtr[ioTextPos] != '\r' )   return false;
						//else ioTextPos++;
					}
					//B0056 一致したのが選択範囲の最後、かつ、それが行頭の場合は不一致と見なす
					if( ioTextPos == inTextTargetEndPos && ioTextPos > 0 )
					{
						if( /*#731 inText.Get(ioTextPos-1)*/inTextPtr[ioTextPos-1] == '\r' )   return false;
					}
					break;
				}
			  case 0x62://単語区切り
				{
					unsigned char	ch1,ch2;
					//B0050 選択範囲でなくテキスト領域全体で単語区切りかどうかを判定すべき
					if( ioTextPos > 0 && ioTextPos < inTextLength )//#731 /*inTextLength*/inText.GetItemCount()) 
					{
						ch1 = inTextPtr[ioTextPos-1];//#731 inText.Get(ioTextPos-1);
						ch2 = inTextPtr[ioTextPos];//#731 inText.Get(ioTextPos);
						if( IsAlphabet(ch1) && IsAlphabet(ch2) )   return false;
						if( !IsAlphabet(ch1) && !IsAlphabet(ch2) )   return false;
					}
					break;
				}
			  case 0x63://単語区切り以外
				{
					unsigned char	ch1,ch2;
					//B0050 選択範囲でなくテキスト領域全体で単語区切りかどうかを判定すべき
					if( ioTextPos > 0 && ioTextPos < inTextLength )//#731 /*inTextLength*/inText.GetItemCount() ) 
					{
						ch1 = inTextPtr[ioTextPos-1];//#731 inText.Get(ioTextPos-1);
						ch2 = inTextPtr[ioTextPos];//#731 inText.Get(ioTextPos);
						if( IsAlphabet(ch1) && !IsAlphabet(ch2) )   return false;
						if( !IsAlphabet(ch1) && IsAlphabet(ch2) )   return false;
					}
					break;
				}
			  case 0x70://後続指定（マッチ）
				{
					mEndPosition = ioTextPos;
					//B0050 選択範囲でなくテキスト領域全体で後続指定一致を判定すべき
					return MatchRecursive(iex,inIexLength,ioIexPos,//#731 inText,/*inTextLength*/
						inTextPtr,inTextLength,inTextLength/*#731 inText.GetItemCount()*/,ioTextPos);
				}
			  case 0x71://後続指定（マッチしない）
				{
					mEndPosition = ioTextPos;
					//B0050 選択範囲でなくテキスト領域全体で後続指定一致を判定すべき
					return !MatchRecursive(iex,inIexLength,ioIexPos,//#731 inText,/*inTextLength*/
						inTextPtr,inTextLength,inTextLength/*#731inText.GetItemCount()*/,ioTextPos);
				}
			}
		}
		else {//制御コード以外
			if( ioTextPos >= inTextTargetEndPos )   return false;
			//R0233 if( ch != inText.Get(ioTextPos) )   return false;
			//R0233
			AUChar	tarch = inTextPtr[ioTextPos];//#731 inText.Get(ioTextPos);
			if( mIgnoreCase == true )
			{
				if( tarch >= 'A' && tarch <= 'Z' )
				{
					tarch += 'a'-'A';
				}
			}
			if( ch != tarch )   return false;
			
			ioTextPos++;
			ioIexPos++;
		}
	}
	return true;
}

ABool	ARegExp::IsAlphabet( const AUChar ch ) const
{
	if( ch >= 'a' && ch <= 'z' )   return true;
	if( ch >= 'A' && ch <= 'Z' )   return true;
	if( ch >= '0' && ch <= '9' )   return true;//B0422
	if( ch == '_' )   return true;
	return false;
}

void	ARegExp::ChangeReplaceText(  const AText& inTargetText, const AText& inText, AText &outText ) const
{
	outText.DeleteAll();
	for( AIndex i = 0; i < inText.GetItemCount(); ) 
	{
		AUChar	ch = inText.Get(i);
		if( ch == '$' && ((i+1) < inText.GetItemCount()) ) 
		{//B0028
			i++;
			AUChar	ch = inText.Get(i);
			AIndex	groupnum;
			if( ch == '&' ) 
			{
				AText	text;
				inTargetText.GetText(mMatchedTextSpos,mMatchedTextEpos-mMatchedTextSpos,text);
				outText.AddText(text);
				i++;
			}
			else if( ch == '+' ) 
			{
				AText	text;
				if( mLastMatchedGroup >= 0 && mLastMatchedGroup < mGroupCount )
				{
					inTargetText.GetText(mGroupSpos[mLastMatchedGroup],mGroupEpos[mLastMatchedGroup]-mGroupSpos[mLastMatchedGroup],text);
				}
				outText.AddText(text);
				i++;
			}
			else if( ParseInteger(inText,i,groupnum) ) 
			{
				if( groupnum == 0 )
				{
					AText	text;
					inTargetText.GetText(mMatchedTextSpos,mMatchedTextEpos-mMatchedTextSpos,text);
					outText.AddText(text);
				}
				else
				{
					groupnum--;
					AText	text;
					if( groupnum >= 0 && groupnum < mGroupCount )
					{
						if( mGroupEpos[groupnum] > mGroupSpos[groupnum] )//#1230 正規表現(A)|(B)で一致して$1$2を置換する場合など、$1のsposのみ設定されてeposが設定されていないことがあるので、その場合は空文字を追加する。
						{
							inTargetText.GetText(mGroupSpos[groupnum],mGroupEpos[groupnum]-mGroupSpos[groupnum],text);
						}
					}
					outText.AddText(text);
				}
			}
		}
		else if( ch == '\\' && ((i+1) < inText.GetItemCount())  )//B0028
		{
			i++;
			ch = inText.Get(i);
			switch(ch)
			{
			  case 'r':
			  case 'n'://#938
				outText.Add('\r');
				break;
			  case 't':
				outText.Add('\t');
				break;
			  default:
				outText.Add(ch);
			}
			i++;
		}
		else 
		{
			outText.Add(ch);
			i++;
		}
	}
}

void	ARegExp::GetGroupRange( AIndex inGroupIndex, AIndex& outStart, AIndex& outEnd ) const
{
	if( inGroupIndex < 0 || inGroupIndex >= kGroupCountMax )
	{
		outStart = outEnd = 0;
		_ACError("",this);
		return;
	}
	if( mGroupEpos[inGroupIndex] > mGroupSpos[inGroupIndex] )//#1230 正規表現(A)|(B)で一致して$1$2を置換する場合など、$1のsposのみ設定されてeposが設定されていないことがあるので、その場合は空範囲を返す。
	{
		outStart = mGroupSpos[inGroupIndex];
		outEnd = mGroupEpos[inGroupIndex];
	}
	else
	{
		outStart = outEnd = 0;
	}
}

//#1231
/**
一致文字列取得
*/
void	ARegExp::GetMatchedText( const AText& inText, AText& outMatchedText ) const
{
	inText.GetText(mMatchedTextSpos,mMatchedTextEpos-mMatchedTextSpos,outMatchedText);
}

//#1231
/**
グループ一致文字列取得
*/
void	ARegExp::GetGroupText( const AText& inText, AIndex inGroupIndex, AText& outGroupText ) const
{
	AIndex	spos = 0, epos = 0;
	GetGroupRange(inGroupIndex,spos,epos);
	inText.GetText(spos,epos-spos,outGroupText);
}

//#437
/**
ワイルドカードから変換
*/
void	ARegExp::ConvertToREFromWildCard( const AText& inWildCard, AText& outRE )
{
	outRE.DeleteAll();
	outRE.AddCstring("(^");
	for( AIndex pos = 0; pos < inWildCard.GetItemCount(); pos++ )
	{
		AUChar	ch = inWildCard.Get(pos);
		switch(ch)
		{
		  case '.':
		  case '\\'://★
		  case '[':
		  case ']':
		  case '^':
		  case '+':
		  case '|':
		  case '$':
		  case '(':
		  case ')':
			{
				outRE.Add(0x5C);
				outRE.Add(ch);
				break;
			}
		  case '*':
			{
				outRE.AddCstring(".*");
				break;
			}
		  case '?':
			{
				outRE.Add('.');
				break;
			}
		  case ';':
			{
				outRE.AddCstring("$)|(^");
				break;
			}
		  default:
			{
				outRE.Add(ch);
				break;
			}
		}
	}
	outRE.AddCstring("$)");
}

#pragma mark -

ALetterRange::ALetterRange() : mStart(NULL,8,8), mEnd(NULL,8,8)//B0373
{
	mNotMode = false;
}

void	ALetterRange::Init()
{
	mStart.DeleteAll();
	mEnd.DeleteAll();
	SetNotMode(false);
}

void	ALetterRange::AddRange( const /*#731 AUTF16Char*/AUCS4Char inStart, const /*#731 AUTF16Char*/AUCS4Char inEnd )
{
	/*#731 AUTF16Char*/AUCS4Char	start = inStart;
	/*#731 AUTF16Char*/AUCS4Char	end = inEnd;
	if( start > end ) 
	{
		/*#731 AUTF16Char*/AUCS4Char	tmp = end;
		end = start;
		start = tmp;
	}
	mStart.Add(start);
	mEnd.Add(end);
}

ABool	ALetterRange::Match( const /*#731 AUTF16Char*/AUCS4Char inLetter ) const
{
	for( AIndex i = 0; i < mStart.GetItemCount(); i++ ) 
	{
		/*#731 AUTF16Char*/AUCS4Char	start,end;
		start = mStart.Get(i);
		end = mEnd.Get(i);
		if( start <= inLetter && inLetter <= end ) 
		{
			return !mNotMode;
		}
	}
	return mNotMode;
}


