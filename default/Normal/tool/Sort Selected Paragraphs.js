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
選択範囲をソート
*/

//＝＝＝＝＝＝＝＝＝＝対象段落取得＝＝＝＝＝＝＝＝＝＝
//現在のドキュメント取得
var doc = mi.getCurrentDocument();
//選択範囲arrayを取得（矩形選択の場合は選択範囲arrayは複数項目になる）
var selectionArray = doc.getSelectionRangeArray();
//対象段落取得
var selstart = doc.getTextPointFromPos(selectionArray[0].start);
var startpara = selstart.y;
var selend = doc.getTextPointFromPos(selectionArray[0].end);
var endpara = selend.y;
if( selend.x == 0 )
{
  //終了位置のxが0のときは、その段落は対象外とする。
  endpara--;
}
//＝＝＝＝＝＝＝＝＝＝テキスト取得＝＝＝＝＝＝＝＝＝＝
var textarray = new Array();
for( var para = startpara; para <= endpara; para++ )
{
  //段落テキスト取得
  var text = doc.getParagraphText(para);
  //最終段落には改行コードが付かないので、改行を付ける
  if( para == doc.getParagraphCount()-1 )
  {
    text += "\r";
  }
  //配列の最後に追加
  textarray.push(text);
}
//＝＝＝＝＝＝＝＝＝＝ソート実行＝＝＝＝＝＝＝＝＝＝
textarray.sort();
//＝＝＝＝＝＝＝＝＝＝ソート済みテキストを対象段落に順に設定＝＝＝＝＝＝＝＝＝＝
for( var para = startpara; para <= endpara; para++ )
{
  //段落範囲取得
  var   range = doc.getParagraphRange(para);
  //配列の最初のテキストを設定して、配列から削除
  doc.setText(range.start,range.end,textarray.shift());
}
//＝＝＝＝＝＝＝＝＝＝対象となった段落を選択＝＝＝＝＝＝＝＝＝＝
doc.setSelectionRange(doc.getPosFromTextPoint(0,startpara),doc.getPosFromTextPoint(0,endpara+1));

