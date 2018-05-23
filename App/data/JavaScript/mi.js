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

//==========アプリケーションクラス==========
var MiApplication = function() { 
    //イベントリスナー
    this.eventListeners = new Array();
};
MiApplication.prototype = {
	//----------ドキュメント取得----------
	//現在編集中のドキュメントオブジェクト取得
    getCurrentDocument : function() { return new TextDocument(miimp.getCurrentDocument()); },
	//新規ドキュメント生成
    createDocument : function() { return new TextDocument(miimp.createDocument()); },
	//ドキュメントUniqIDからドキュメントオブジェクト取得
    getDocumentByID : function(docID) { return new TextDocument(miimp.getDocumentByID(docID)); },
	//----------スクリプト制御----------
	//モジュールロード
    loadModule : function(name) { miimp.loadModule(name); },
    //----------イベントリスナー----------
    //イベントリスナー削除
	//modeName: String: イベントリスナー実行対象のモードの名称
	//id: String : プラグイン識別のためのID
    removeEventListeners : function (modeName, id) {
        //現在のイベントリスナー配列から同じmodeName,idのイベントリスナーは削除して、新規配列へコピー
        var newArray = new Array();
        for( var i = 0; i < this.eventListeners.length; i++ ) {
            if( !(this.eventListeners[i].modeName == modeName && this.eventListeners[i].id == id) ) {
                newArray.push(this.eventListeners[i]);
        }}
        //新規配列の内容をイベントリスナーに設定
        this.eventListeners = newArray;
    },
    //イベントリスナー追加
    //modeName: String : イベントリスナー実行対象のモードの名称
    //id: String : プラグイン識別のためのID
    //events: Array of String : 対象イベント名
    //fn: function : 実行する関数 function(doc,event,parameter) doc:Object:対象ドキュメント event:String:イベント名 parameter:String:パラメータ
    addEventListener : function(modeName, id, events, fn) {
        //modeName,idのイベントリスナー削除
        this.removeEventListeners(modeName,id);
        //イベントごとにイベントリスナー追加
        for( var j = 0; j < events.length; j++ ) {
            var eventListener = { modeName:modeName, event:events[j], id: id, fn: fn };
            this.eventListeners.push(eventListener);
        }
    },
    //イベントリスナー実行（基本的にmi内部から実行されます。ツール／プラグインから使用する必要はありません。）
    //doc: Object : 対象ドキュメント
    //event: String : イベント名
	//parameter: String : パラメータ
    executeEventListeners : function(doc, event, parameter) { 
        //ドキュメントのモード名取得
        var modeName = doc.getModeName();
        //イベントリスナー実行
        for( var i = 0; i < this.eventListeners.length; i++ ) {
            if( this.eventListeners[i].modeName == modeName && this.eventListeners[i].event == event ) {
                this.eventListeners[i].fn(doc,event,parameter);
            }
    }},
    //----------プラグイン----------
    //プラグイン登録
    //modeName: String: 対象モード名
    //id: String: プラグイン識別のためのID
	//name: プラグイン名
    //defaultEnabled: Boolean: デフォルトでEnableかどうか
	//defaultOptions: String: デフォルトのオプション文字列
    registerPlugin : function(modeName,id,name,defaultEnabled,defaultOptions) { return miimp.registerPlugin(modeName,id,name,defaultEnabled,defaultOptions); },
    //プラグイン表示名
    //modeName: String: 対象モード名
    //lang: 言語名
    //displayName: プラグイン表示名（モード設定ウインドウのプラグインEnable/Disable設定にて表示する名称）
    setPluginDisplayName : function(modeName,id,lang,displayName) { return miimp.setPluginDisplayName(modeName,id,lang,displayName); },
    //プラグインがEnable設定かどうかを取得
    //modeName: String: 対象モード名
    //id: String: プラグイン識別のためのID
    getPluginEnabled : function(modeName,id) { return miimp.getPluginEnabled(modeName,id); },
    //プラグインの（ユーザーが設定した）オプション文字列を取得
	//modeName: String: 対象モード名
    //id: String: プラグイン識別のためのID
    getPluginOptions : function(modeName,id) { return miimp.getPluginOptions(modeName,id); },
    //現在ロード中の対象モードの名称を取得
    getCurrentModeNameForPluginLoad : function() { return miimp.getCurrentModeNameForPluginLoad(); },
	//----------ダイアログ----------
	showDialog : function(title,message) { return miimp.showDialog(title,message); },
	showDialog : function(title,message,okButton) { return miimp.showDialog(title,message,okButton); },
	showDialog : function(title,message,okButton,cancelButton) { return miimp.showDialog(title,message,okButton,cancelButton); },
    //----------デバッグ用----------
    //標準エラー出力
    outputToStderr : function(text) { miimp.outputToStderr(text); },
};

//Document prototype
var TextDocument = function(docimp) { this._doc = docimp; };
TextDocument.prototype = {
	//----------テキスト編集----------
    getLength : function() { return this._doc.getLength(); },
    getParagraphCount : function() { return this._doc.getParagraphCount(); },
    getContent : function() { var sel = {start:0, end:this._doc.getLength()}; return this._doc.getText(sel.start,sel.end); },
    getSelectionRange : function(selindex) { return this._doc.getSelectionRange(selindex); },
    getSelectionRangeArray : function() { 
        var array = new Array(); 
        var count = this._doc.getSelectionCount();
        for( var i = 0; i < count; i++ )
        {
            array.push(this._doc.getSelectionRange(i));
        }
        return array;
    },
    getParagraphRange : function(para) { return this._doc.getParagraphRange(para); },
    getParagraphText : function(para) { var range = this._doc.getParagraphRange(para); return this._doc.getText(range.start,range.end); },
    getTextPointFromPos : function(pos) { return this._doc.getTextPointFromPos(pos); },
    getPosFromTextPoint : function(x,y) { return this._doc.getPosFromTextPoint(x,y); },
    getSelectionCount : function() { return this._doc.getSelectionCount(); },
    getText : function(start,end) { return this._doc.getText(start,end); },
    getSelectionText : function() { var sel = this._doc.getSelectionRange(); return this._doc.getText(sel.start,sel.end); },
    setSelectionRange : function(start,end) { return this._doc.setSelectionRange(start,end); },
    setText : function(start,end,text) { return this._doc.setText(start,end,text); },
    setParagraphText : function(para,text) { var range = this._doc.getParagraphRange(para); this._doc.setText(range.start,range.end,text); },
    getSyntaxStateNameFromPos : function(pos) { return this._doc.getSyntaxStateNameFromPos(pos); },
    setContent : function(text) { this._doc.setText(0,this._doc.getLength(),text); },
    getCaretPos : function() { return this._doc.getSelectionRange().start; },
    setCaretPos : function(pos) { this._doc.setSelectionRange(pos,pos); },
    //----------ドキュメント情報----------
    getModeName : function() { return this._doc.getModeName(); },
    getURL : function() { return this._doc.getURL(); },
    //----------文法チェッカー結果情報設定----------
    clearSyntaxCheckerWarnings : function() { return this._doc.clearSyntaxCheckerWarnings(); },
    addSyntaxCheckerWarning : function(para,col,title,detail,type,displayInTextView) { return this._doc.addSyntaxCheckerWarning(para,col,title,detail,type,displayInTextView); },
    //----------その他----------
    setDocInfoText : function(text) { return this._doc.setDocInfoText(text); }
};

//"mi" variable definition
var mi = new MiApplication();

