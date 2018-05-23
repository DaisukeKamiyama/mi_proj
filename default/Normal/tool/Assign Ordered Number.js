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
選択範囲をナンバリング（選択範囲内の数値文字列を、順に＋１するように置換）
*/

//現在のドキュメント取得
var doc = mi.getCurrentDocument();
//選択範囲arrayを取得（矩形選択の場合は選択範囲arrayは複数項目になる）
var selectionArray = doc.getSelectionRangeArray();
//現在数値を初期化
var currentnum = undefined;
//置換テキストarray（＝各選択範囲に対応）初期化
var repTextArray = new Array();
//----------各選択範囲ごとのループ----------
for( var i = 0; i < selectionArray.length; i++ )
{
  //選択範囲取得
  var range = selectionArray[i];
  //選択範囲テキスト取得
  var text = doc.getText(range.start,range.end);
  //数値部分の開始位置（状態：「数値文字列内」フラグを兼用）初期化
  var numspos = -1;
  //置換後テキスト初期化
  var reptext = "";
  //置換後テキストに追加済みの最終位置
  var lastpos = 0;
  //----------文字毎のループ----------
  //置換後テキストreptextに、選択範囲の置換後テキストを追加していく。
  for( var pos = 0; pos < text.length; pos++ )
  {
    //文字取得
    var ch = text.charAt(pos);
    //現在、状態：「数値文字列内」かどうかの分岐
    if( numspos == -1 )
    {
      //----------現在、状態：「数値文字内」ではない場合----------
      //今回の文字が0〜9なら、状態：「数値文字列内」へ移行。開始位置を記憶。
      if( (ch >= '0' && ch <= '9') )
      {
        numspos = pos;
      }
    }
    else
    {
      //----------現在、状態：「数値文字内」の場合----------
      //今回の文字が0〜9以外なら、置換後テキストに追加して、状態：「数値文字列内」を解除
      if( !( (ch >= '0' && ch <= '9') ) )
      {
        //数値部分のテキストを取得
        var numtext = text.substring(numspos,pos);
        //数値部分の前までのテキストを、そのまま置換後テキストに追加
        reptext += text.substring(lastpos,numspos);
        lastpos = pos;
        //現在数値を更新
        if( currentnum == undefined )
        {
          currentnum = parseInt(numtext);
        }
        else
        {
          currentnum++;
        }
        //置換後テキストに現在数値をテキスト化して追加
        reptext += currentnum;
        //状態：「数値文字列内」を解除
        numspos = -1;
      }
    }
  }
  //現在、状態：「数値文字列内」かどうかの分岐
  if( numspos == -1 )
  {
    //----------現在、状態：「数値文字内」ではない場合----------
    //最後の数値部分の後のテキストを、そのまま置換後テキストに追加
    reptext += text.substring(lastpos,text.length);
  }
  else
  {
    //----------現在、状態：「数値文字内」の場合----------
    //数値部分のテキストを取得
    var numtext = text.substring(numspos,pos);
    //数値部分の前までのテキストを、そのまま置換後テキストに追加
    reptext += text.substring(lastpos,numspos);
    lastpos = pos;
    //現在数値を更新
    if( currentnum == undefined )
    {
      currentnum = parseInt(numtext);
    }
    else
    {
      currentnum++;
    }
    //置換後テキストに現在数値をテキスト化して追加
    reptext += currentnum;
    //状態：「数値文字列内」を解除
    numspos = -1;
  }
  //置換テキストarrayに追加
  repTextArray.push(reptext);
}
//----------各選択範囲を置換----------
//※置換後、selectionArrayを更新しなくて良いように、後ろから置換している。
for( var i = selectionArray.length-1; i >= 0 ; i-- )
{
  //選択範囲取得
  var range = selectionArray[i];
  //置換実行
  doc.setText(range.start,range.end,repTextArray.pop());
}

