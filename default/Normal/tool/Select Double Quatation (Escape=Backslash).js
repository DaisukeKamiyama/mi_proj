/*

Copyright (c) 2012, Daisuke Kamiyama 
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, 
 this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright notice, 
 this list of conditions and the following disclaimer in the documentation 
 and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

*/

/*
ダブルクオーテーション内を選択
エスケープ文字：バックスラッシュの場合
*/

//現在のドキュメント取得
var doc = mi.getCurrentDocument();
//現在のキャレット位置を取得
var caretpos = doc.getCaretPos();
//全体の長さを取得
var len = doc.getLength();
//結果初期化
var spos = 0;
var epos = len;
//----------開始位置検索ループ----------
for( var pos = caretpos-1; pos > 0; pos-- )
{
  //現在位置の文字がダブルクオーテーションかどうかの判定
  if( doc.getText(pos,pos+1)  == "\"" )
  {
    //現在位置の前にエスケープ文字があるかどうかの判定
    if( pos-1 >= 0 )
    {
      if( doc.getText(pos-1,pos) == "\\" )
      {
        //前にエスケープ文字があれば、ループ継続
        pos--;
        continue;
      }
    }
    //sposにダブルクオーテーション直後の位置を設定してループ終了
    spos = pos+1;
    break;
  }
}
//----------終了位置検索ループ----------
for( var pos = caretpos; pos < len; pos++ )
{
  //現在位置の文字がエスケープ文字かどうかの判定
  var   ch = doc.getText(pos,pos+1);
  if( ch == "\\" )
  {
    //エスケープ文字なら次の文字を飛ばしてループ継続
    if( pos+1 < len )
    {
      pos++;
    }
    continue;
  }
  //現在位置の文字がダブルクオーテーションかどうかの判定
  if( ch == "\"" )
  {
    //eposにダブルクオーテーション直前の位置を設定してループ終了
    epos = pos;
    break;
  }
}

//----------spos〜eposを選択----------
doc.setSelectionRange(spos,epos);


