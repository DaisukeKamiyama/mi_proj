/*

Copyright (c) 2013 Daisuke Kamiyama
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

//プラグインのユニークID
var pluginId = "std.jshint.mimikaki.net";

//プラグイン登録（デフォルトenabled）
var modeName = mi.getCurrentModeNameForPluginLoad();
mi.registerPlugin(modeName,pluginId,"JSHINT",true,"");
mi.setPluginDisplayName(modeName,pluginId,"ja","JSHINT (JavaScript 文法チェック)");

//プラグインEnable設定を取得し、Enableなら、イベントリスナーを登録
if( mi.getPluginEnabled(modeName,pluginId) === true )
{
    //JSHINTをロード
    mi.loadModule('./script/jshint.js');
    
    //イベントリスナー追加
    mi.addEventListener(modeName,pluginId,["onSaved","onOpened"],
        function(doc,event,parameter) {
            //プラグインオプション設定を取得
            var options = mi.getPluginOptions(modeName,pluginId);
            //ドキュメントのURLを取得
            var url = doc.getURL();
            //拡張子が.jsの場合のみ実行
            if( url.substr(url.length-3,3) == ".js" )
            {
                //文法チェッカーワーニングを全て消去
                doc.clearSyntaxCheckerWarnings();
                //JSHINT実行
                var res = JSHINT(doc.getContent(),options);
                //ワーニング項目ごとのループ
                for( var i = 0; i < JSHINT.errors.length; i++ )
                {
                    //文法チェッカーワーニング追加
                    var lineIndex = JSHINT.errors[i].line-1;
                    var colIndex = JSHINT.errors[i].character-1;
                    var reason = JSHINT.errors[i].reason;
                    var detail = "JSHINT\n"+
                    JSHINT.errors[i].reason+"\n"+
                    "id: "+JSHINT.errors[i].id+"\n"+
                    "raw: "+JSHINT.errors[i].raw+"\n"+
                    "evidence: "+JSHINT.errors[i].evidence+"\n"+
                    "character: "+JSHINT.errors[i].character+"\n"+
                    "scope: "+JSHINT.errors[i].scope+"\n"+
                    "a: "+JSHINT.errors[i].a+"\n"+
                    "b: "+JSHINT.errors[i].b+"\n"+
                    "c: "+JSHINT.errors[i].c+"\n"+
                    "d: "+JSHINT.errors[i].d+"\n";
                    doc.addSyntaxCheckerWarning(lineIndex,colIndex,reason,detail,2,true);//2:ワーニング、true:テキストエリア内にワーニング表示
                }
            }
    });
}
else {
    //プラグインDisableならこのプラグインのイベントリスナーを全て削除
    mi.removeEventListeners(modeName,pluginId);
}


