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
選択範囲のテキストをUnicodeのhex表示として、文字に変換
*/

//現在のドキュメント取得
var doc = mi.getCurrentDocument();
//選択範囲arrayを取得（矩形選択の場合は選択範囲arrayは複数項目になる）
var selectionArray = doc.getSelectionRangeArray();
//結果テキスト初期化
var resulttext = "";
//選択テキスト取得
var seltext = doc.getSelectionText();
//カンマを区切り文字としてテキスト配列取得
var seltextarray = seltext.split(",");
//配列内項目ループ
for( var i = 0; i < seltextarray.length; i++ )
{
	//テキストを16進数として数値に変換
	var unicode = parseInt(seltextarray[i],16);
	//その16進数をUnicodeとして、resulttextに追加
	resulttext += String.fromCharCode(unicode);
}
//選択範囲を結果テキストで置き換え
doc.setText(selectionArray[0].start,selectionArray[0].end,resulttext);
//置き換え後のテキストを選択
doc.setSelectionRange(selectionArray[0].start,selectionArray[0].start+resulttext.length);

