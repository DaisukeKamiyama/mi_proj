/**
 * Middleware layer that communicates between editor and Zen Coding.
 * This layer describes all available Zen Coding actions, like 
 * "Expand Abbreviation".
 * @author Sergey Chikuyonok (serge.che@gmail.com)
 * @link http://chikuyonok.ru
 * 
 * @include "editor.js"
 * @include "../../aptana/lib/html_matcher.js"
 * @include "../../aptana/lib/zen_coding.js"
 */

/**
 * Search for abbreviation in editor from current caret position
 * @param {zen_editor} editor Editor instance
 * @return {String|null}
 */
function findAbbreviation(editor) {
	var range = editor.getSelectionRange();
	if (range.start != range.end) {
		// abbreviation is selected by user
		return editor.getContent().substring(range.start, range.end);
	}
	
	// search for new abbreviation from current caret position
	var cur_line = editor.getCurrentLineRange();
	return zen_coding.extractAbbreviation(editor.getContent().substring(cur_line.start, range.start));
}

/**
 * Find from current caret position and expand abbreviation in editor
 * @param {zen_editor} editor Editor instance
 * @param {String} type Syntax type (html, css, etc.)
 * @param {String} profile_name Output profile name (html, xml, xhtml)
 * @return {Boolean} Returns <code>true</code> if abbreviation was expanded 
 * successfully
 */
function expandAbbreviation(editor, type, profile_name) {
	profile_name = profile_name || 'xhtml';
	
	var caret_pos = editor.getSelectionRange().end,
		abbr,
		content = '';
		
	if ( (abbr = findAbbreviation(editor)) ) {
		content = zen_coding.expandAbbreviation(abbr, type, profile_name);
		if (content) {
			editor.replaceContent(content, caret_pos - abbr.length, caret_pos);
			return true;
		}
	}
	
	return false;
}

/**
 * A special version of <code>expandAbbreviation</code> function: if it can't
 * find abbreviation, it will place Tab character at caret position
 * @param {zen_editor} editor Editor instance
 * @param {String} type Syntax type (html, css, etc.)
 * @param {String} profile_name Output profile name (html, xml, xhtml)
 */
function expandAbbreviationWithTab(editor, type, profile_name) {
	if (!expandAbbreviation(editor, type, profile_name))
		editor.replaceContent('\t', editor.getCaretPos());
}

/**
 * Find and select HTML tag pair
 * @param {zen_editor} editor Editor instance
 * @param {String} [direction] Direction of pair matching: 'in' or 'out'. 
 * Default is 'out'
 */
function matchPair(editor, direction) {
	direction = (direction || 'out').toLowerCase();
	
	var range = editor.getSelectionRange(),
		cursor = range.end,
		range_start = range.start, 
		range_end = range.end,
//		content = zen_coding.splitByLines(editor.getContent()).join('\n'),
		content = editor.getContent(),
		range = null,
		_r,
	
		old_open_tag = HTMLPairMatcher.last_match['opening_tag'],
		old_close_tag = HTMLPairMatcher.last_match['closing_tag'];
		
	if (direction == 'in' && old_open_tag && range_start != range_end) {
//		user has previously selected tag and wants to move inward
		if (!old_close_tag) {
//			unary tag was selected, can't move inward
			return false;
		} else if (old_open_tag.start == range_start) {
			if (content[old_open_tag.end] == '<') {
//				test if the first inward tag matches the entire parent tag's content
				_r = HTMLPairMatcher.find(content, old_open_tag.end + 1);
				if (_r[0] == old_open_tag.end && _r[1] == old_close_tag.start) {
					range = HTMLPairMatcher(content, old_open_tag.end + 1);
				} else {
					range = [old_open_tag.end, old_close_tag.start];
				}
			} else {
				range = [old_open_tag.end, old_close_tag.start];
			}
		} else {
			var new_cursor = content.substring(0, old_close_tag.start).indexOf('<', old_open_tag.end);
			var search_pos = new_cursor != -1 ? new_cursor + 1 : old_open_tag.end;
			range = HTMLPairMatcher(content, search_pos);
		}
	} else {
		range = HTMLPairMatcher(content, cursor);
	}
	
	if (range !== null && range[0] != -1) {
//		alert(range[0] + ', '+ range[1]);
		editor.createSelection(range[0], range[1]);
		return true;
	} else {
		return false;
	}
}

/**
 * Wraps content with abbreviation
 * @param {zen_editor} Editor instance
 * @param {String} type Syntax type (html, css, etc.)
 * @param {String} profile_name Output profile name (html, xml, xhtml)
 */
function wrapWithAbbreviation(editor, abbr, type, profile_name) {
	profile_name = profile_name || 'xhtml';
	
	var range = editor.getSelectionRange(),
		start_offset = range.start,
		end_offset = range.end,
		content = editor.getContent();
		
		
	if (!abbr)
		return null; 
	
	if (start_offset == end_offset) {
		// no selection, find tag pair
		range = HTMLPairMatcher(content, start_offset);
		
		if (!range || range[0] == -1) // nothing to wrap
			return null;
			
		start_offset = range[0];
		end_offset = range[1];
			
		// narrow down selection until first non-space character
		var re_space = /\s|\n|\r/;
		function isSpace(ch) {
			return re_space.test(ch);
		}
		
		while (start_offset < end_offset) {
			if (!isSpace(content.charAt(start_offset)))
				break;
				
			start_offset++;
		}
		
		while (end_offset > start_offset) {
			end_offset--;
			if (!isSpace(content.charAt(end_offset))) {
				end_offset++;
				break;
			}
		}
			
	}
	
	var new_content = content.substring(start_offset, end_offset),
		result = zen_coding.wrapWithAbbreviation(abbr, unindent(editor, new_content), type, profile_name);
	
	if (result) {
		editor.createSelection(end_offset);
		editor.replaceContent(result, start_offset, end_offset);
	}
}

/**
 * Unindent content, thus preparing text for tag wrapping
 * @param {zen_editor} Editor instance
 * @param {String} text
 * @return {String}
 */
function unindent(editor, text) {
	var pad = getCurrentLinePadding(editor);
	var lines = zen_coding.splitByLines(text);
	for (var i = 0; i < lines.length; i++) {
		if (lines[i].search(pad) == 0)
			lines[i] = lines[i].substr(pad.length);
	}
	
	return lines.join(zen_coding.getNewline());
}

/**
 * Returns padding of current editor's line
 * @param {zen_editor} Editor instance
 * @return {String}
 */
function getCurrentLinePadding(editor) {
	return (editor.getCurrentLine().match(/^(\s+)/) || [''])[0];
}

/**
 * Search for new caret insertion point
 * @param {zen_editor} editor Editor instance
 * @param {Number} Search increment: -1 — search left, 1 — search right
 * @param {Number} Initial offset relative to current caret position
 * @return {Number} Returns -1 if insertion point wasn't found
 */
function findNewEditPoint(editor, inc, offset) {
	inc = inc || 1;
	offset = offset || 0;
	var cur_point = editor.getCaretPos() + offset,
		content = editor.getContent(),
		max_len = content.length,
		next_point = -1,
		re_empty_line = /^\s+$/;
	
	function ch(ix) {
		return content.charAt(ix);
	}
	
	function getLine(ix) {
		var start = ix;
		while (start >= 0) {
			var c = ch(start);
			if (c == '\n' || c == '\r')
				break;
			start--;
		}
		
		return content.substring(start, ix);
	}
		
	while (cur_point < max_len && cur_point > 0) {
		cur_point += inc;
		var cur_char = ch(cur_point),
			next_char = ch(cur_point + 1),
			prev_char = ch(cur_point - 1);
			
		switch (cur_char) {
			case '"':
			case '\'':
				if (next_char == cur_char && prev_char == '=') {
					// empty attribute
					next_point = cur_point + 1;
				}
				break;
			case '>':
				if (next_char == '<') {
					// between tags
					next_point = cur_point + 1;
				}
				break;
			case '\n':
			case '\r':
				// empty line
				if (re_empty_line.test(getLine(cur_point - 1))) {
					next_point = cur_point;
				}
				break;
		}
		
		if (next_point != -1)
			break;
	}
	
	return next_point;
}

/**
 * Move caret to previous edit point
 * @param {zen_editor} editor Editor instance
 */
function prevEditPoint(editor) {
	var cur_pos = editor.getCaretPos(),
		new_point = findNewEditPoint(editor, -1);
		
	if (new_point == cur_pos)
		// we're still in the same point, try searching from the other place
		new_point = findNewEditPoint(editor, -1, -2);
	
	if (new_point != -1) 
		editor.createSelection(new_point);
}

/**
 * Move caret to next edit point
 * @param {zen_editor} editor Editor instance
 */
function nextEditPoint(editor) {
	var new_point = findNewEditPoint(editor, 1);
	if (new_point != -1)
		editor.createSelection(new_point);
}

/**
 * Inserts newline character with proper indentation
 * @param {zen_editor} editor Editor instance
 * @param {String} mode Syntax mode (only 'html' is implemented)
 */
function insertFormattedNewline(editor, mode) {
	mode = mode || 'html';
	var pad = getCurrentLinePadding(editor),
		caret_pos = editor.getCaretPos();
		
	function insert_nl() {
		editor.replaceContent('\n', caret_pos);
	}
	
	switch (mode) {
		case 'html':
			// let's see if we're breaking newly created tag
			var pair = HTMLPairMatcher.getTags(editor.getContent(), editor.getCaretPos());
			
			if (pair[0] && pair[1] && pair[0].type == 'tag' && pair[0].end == caret_pos && pair[1].start == caret_pos) {
				editor.replaceContent('\n\t|\n', caret_pos);
			} else {
				insert_nl();
			}
			break;
		default:
			insert_nl();
	}
}

/**
 * Select line under cursor
 * @param {zen_editor} editor Editor instance
 */
function selectLine(editor) {
	var range = editor.getCurrentLineRange();
	editor.createSelection(range.start, range.end);
}