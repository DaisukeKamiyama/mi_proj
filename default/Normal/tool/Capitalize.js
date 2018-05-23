// toolname-ja="キャピタライズ", toolname-en="Capitalize"
/*

Copyright (c) 2014, Daisuke Kamiyama 
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
キャピタライズ
*/

//キャピタライズ
function capitalize(text)
{
  return text.replace( /(^|\W)([\w])/g , function(all,g1,g2){ return g1+g2.toUpperCase(); } );
}

//現在のドキュメント取得
var doc = mi.getCurrentDocument();
//選択範囲arrayを取得（矩形選択の場合は選択範囲arrayは複数項目になる）
var selectionArray = doc.getSelectionRangeArray();
//置換テキストarray（＝各選択範囲に対応）初期化
var repTextArray = new Array();
//----------各選択範囲ごとのループ----------
for( var i = 0; i < selectionArray.length; i++ )
{
  //選択範囲取得
  var range = selectionArray[i];
  //選択範囲テキスト取得
  var text = doc.getText(range.start,range.end);
  //キャピタライズ
  var reptext = capitalize(text);
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

