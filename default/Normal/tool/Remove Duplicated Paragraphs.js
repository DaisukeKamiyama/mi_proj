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
//段落毎のループ
for( var paraIndex = 0; paraIndex < doc.getParagraphCount(); paraIndex++ )
{
  //対象段落のテキストを取得
  var text = doc.getParagraphText(paraIndex);
  //対象段落以降の段落のループ
  for( var p = paraIndex+1; p < doc.getParagraphCount(); )
  {
    //段落のテキストを取得
    var t = doc.getParagraphText(p);
    if( text === t )
    {
      //同じテキストの場合、段落を削除（空文字列で置き換え）
      doc.setParagraphText(p,"");
    }
    else
    {
      //同じテキストではない場合、次の段落へ
      p++;
    }
  }
}
