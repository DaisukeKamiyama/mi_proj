/**
* High-level editor interface which communicates with other editor (like 
* TinyMCE, CKEditor, etc.) or browser.
* Before using any of editor's methods you should initialize it with
* <code>editor.setTarget(elem)</code> method and pass reference to 
* &lt;textarea&gt; element.
* @example
* var textarea = document.getElemenetsByTagName('textarea')[0];
* editor.setTarget(textarea);
* //now you are ready to use editor object
* editor.getSelectionRange() 
* 
* @author Sergey Chikuyonok (serge.che@gmail.com)
* @link http://chikuyonok.ru
* @include "../../aptana/lib/zen_coding.js"
*/
var zen_editor = (function(){
    /** @param {TextDocument} target document */
    var doc = null;
    
    zen_coding.setNewline('\r');
    
    /**
    * Returns content of current target element
    */
    function getContent() {
      return doc.getContent();
    }
    
    /**
    * Returns selection indexes from element
    */
    function getSelectionRange() {
      return doc.getSelectionRange();
    }
    
    /**
    * Creates text selection on target element
    * @param {Number} start
    * @param {Number} end
    */
    function createSelection(start, end) {
      return doc.setSelectionRange(start,end);
    }
    
    /**
    * Returns current caret position
    */
    function getCaretPos() {
      return doc.getCaretPos();
    }
    
    /**
    * Returns whitrespace padding of string
    * @param {String} str String line
    * @return {String}
    */
    function getStringPadding(str) {
      return (str.match(/^(\s+)/) || [''])[0];
    }
    
    return {
      setCurrentDocument: function(d) {
        doc = d;
      },
      
      getSelectionRange: getSelectionRange,
      createSelection: createSelection,
      
      /**
      * Returns current line's start and end indexes
      */
      getCurrentLineRange: function() {
        return doc.getParagraphRange(doc.getTextPointFromPos(doc.getCaretPos()).y);
      },
      
      /**
      * Returns current caret position
      * @return {Number}
      */
      getCaretPos: getCaretPos,
      
      /**
      * Returns content of current line
      * @return {String}
      */
      getCurrentLine: function() {
        return doc.getParagraphText(doc.getTextPointFromPos(doc.getCaretPos()).y);
      },
      
      /**
      * Replace editor's content or it's part (from <code>start</code> to 
      * <code>end</code> index). If <code>value</code> contains 
      * <code>caret_placeholder</code>, the editor will put caret into 
      * this position. If you skip <code>start</code> and <code>end</code>
      * arguments, the whole target's content will be replaced with 
      * <code>value</code>. 
      * 
      * If you pass <code>start</code> argument only,
      * the <code>value</code> will be placed at <code>start</code> string 
      * index of current content. 
      * 
      * If you pass <code>start</code> and <code>end</code> arguments,
      * the corresponding substring of current target's content will be 
      * replaced with <code>value</code>. 
      * @param {String} value Content you want to paste
      * @param {Number} [start] Start index of editor's content
      * @param {Number} [end] End index of editor's content
      */
      replaceContent: function(value, start, end) {
        doc.setText(start,end,value);
      },
      
      /**
      * Returns editor's content
      * @return {String}
      */
      getContent: getContent
    }
})();
