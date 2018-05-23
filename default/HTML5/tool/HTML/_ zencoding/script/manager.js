/**
 * Editor manager that handles all incoming events and runs Zen Coding actions.
 * This manager is also used for setting up editor preferences
 * @author Sergey Chikuyonok (serge.che@gmail.com)
 * @link http://chikuyonok.ru
 * 
 * @include "actions.js"
 * @include "editor.js"
 * @include "shortcut.js"
 */zen_textarea = (function(){ // should be global
	var default_options = {
		profile: 'xhtml',
		syntax: 'html',
		use_tab: false,
		pretty_break: false
	},
	
	mac_char_map = {
		'ctrl': '⌃',
		'control': '⌃',
		'meta': '⌘',
		'shift': '⇧',
		'alt': '⌥',
		'enter': '⏎',
		'tab': '⇥',
		'left': '←',
		'right': '→'
	},
	
	pc_char_map = {
		'left': '←',
		'right': '→'
	},
	
	shortcuts = {},
	is_mac = /mac\s+os/i.test(navigator.userAgent),
	
	/** Zen Coding parameter name/value regexp for getting options from element */
	re_param = /\bzc\-(\w+)\-(\w+)/g;
	
	/** @type {default_options} */
	var options = {};
	
	function copyOptions(opt) {
		opt = opt || {};
		var result = {};
		for (var p in default_options) if (default_options.hasOwnProperty(p)) {
			result[p] = (p in opt) ? opt[p] : default_options[p];
		}
		
		return result;
	}
	
	options = copyOptions();
	
	/**
	 * Makes first letter of string in uppercase
	 * @param {String} str
	 */
	function capitalize(str) {
		return str.charAt().toUpperCase() + str.substring(1);
	}
	
	function humanize(str) {
		return capitalize(str.replace(/_(\w)/g, function(s, p){return ' ' + p.toUpperCase()}));
	}
	
	function formatShortcut(char_map, glue) {
		var result = [];
		if (typeof(glue) == 'undefined')
			glue = '+';
			
		for (var p in shortcuts) if (shortcuts.hasOwnProperty(p)) {
			var keys = p.split('+'),
				ar = [],
				lp = p.toLowerCase();
				
			if (lp == 'tab' || lp == 'enter')
				continue;
				
			for (var i = 0; i < keys.length; i++) {
				var key = keys[i].toLowerCase();
				ar.push(key in char_map ? char_map[key] : capitalize(key));
			}
			
			result.push({
				'keystroke': ar.join(glue), 
				'action_name': humanize(shortcuts[p])
			});
		}
		
		return result;
	}
	
	
	/**
	 * Get Zen Coding options from element's class name
	 * @param {Element} elem
	 */
	function getOptionsFromElement(elem) {
		var param_str = elem.className || '',
			m,
			result = copyOptions(options);
			
		while ( (m = re_param.exec(param_str)) ) {
			var key = m[1].toLowerCase(),
				value = m[2].toLowerCase();
			
			if (value == 'true' || value == 'yes' || value == '1')
				value = true;
			else if (value == 'false' || value == 'no' || value == '0')
				value = false;
				
			result[key] = value;
		}
		
		return result;
	}
	
	/**
	 * Returns normalized action name
	 * @param {String} name Action name (like 'Expand Abbreviation')
	 * @return Normalized name for coding (like 'expand_abbreviation')
	 */
	function normalizeActionName(name) {
		return name
			.replace(/(^\s+|\s+$)/g, '') // remove trailing spaces
			.replace(/\s+/g, '_')
			.toLowerCase();
	}
	
	/**
	 * Runs actions called by user
	 * @param {String} name Normalized action name
	 * @param {Event} evt Event object
	 */
	function runAction(name, evt) {
		/** @type {Element} */
		var target_elem = evt.target || evt.srcElement,
			key_code = evt.keyCode || evt.which;
			
		if (target_elem && target_elem.nodeType == 1 && target_elem.nodeName == 'TEXTAREA') {
			zen_editor.setTarget(target_elem);
			
			var options = getOptionsFromElement(target_elem),
				syntax = options.syntax,
				profile_name = options.profile;
			
			switch (name) {
				case 'expand_abbreviation':
					if (key_code == 9) {
						if (options.use_tab)
							expandAbbreviationWithTab(zen_editor, syntax, profile_name);
						else
							// user pressed Tab key but it's forbidden in 
							// Zen Coding: bubble up event
							return true;
							
					} else {
						expandAbbreviation(zen_editor, syntax, profile_name);
					}
					break;
				case 'match_pair_inward':
				case 'balance_tag_inward':
					matchPair(zen_editor, 'in');
					break;
				case 'match_pair_outward':
				case 'balance_tag_outward':
					matchPair(zen_editor, 'out');
					break;
				case 'wrap_with_abbreviation':
					var abbr = prompt('Enter abbreviation', 'div');
					if (abbr)
						wrapWithAbbreviation(zen_editor, abbr, syntax, profile_name);
					break;
				case 'next_edit_point':
					nextEditPoint(zen_editor);
					break;
				case 'previous_edit_point':
				case 'prev_edit_point':
					prevEditPoint(zen_editor);
					break;
				case 'pretty_break':
				case 'format_line_break':
					if (key_code == 13) {
						if (options.pretty_break)
							insertFormattedNewline(zen_editor);
						else
							// user pressed Enter but it's forbidden in 
							// Zen Coding: bubble up event
							return true;
					} else {
						insertFormattedNewline(zen_editor);
					}
					break;
				case 'select_line':
					selectLine(zen_editor);
			}
		} else {
			// allow event bubbling
			return true;
		}
	}
	
	/**
	 * Bind shortcut to Zen Coding action
	 * @param {String} keystroke
	 * @param {String} action_name
	 */
	function addShortcut(keystroke, action_name) {
		action_name = normalizeActionName(action_name);
		shortcuts[keystroke.toLowerCase()] = action_name;
		shortcut.add(keystroke, function(evt){
			return runAction(action_name, evt);
		});
	}
	
	// add default shortcuts
	addShortcut('Meta+E', 'Expand Abbreviation');
	addShortcut('Tab', 'Expand Abbreviation');
	addShortcut('Meta+D', 'Balance Tag Outward');
	addShortcut('Shift+Meta+D', 'Balance Tag inward');
	addShortcut('Shift+Meta+A', 'Wrap with Abbreviation');
	addShortcut('Ctrl+Alt+RIGHT', 'Next Edit Point');
	addShortcut('Ctrl+Alt+LEFT', 'Previous Edit Point');
	addShortcut('Meta+L', 'Select Line');
	addShortcut('Enter', 'Format Line Break');
	
	
	return {
		shortcut: addShortcut,
		
		/**
		 * Removes shortcut binding
		 * @param {String} keystroke
		 */
		unbindShortcut: function(keystroke) {
			keystroke = keystroke.toLowerCase();
			if (keystroke in shortcuts)
				delete shortcuts[keystroke];
			shortcut.remove(keystroke);
		},
		
		/**
		 * Setup editor. Pass object with values defined in 
		 * <code>default_options</code>
		 */
		setup: function(opt) {
			options = copyOptions(opt);
		},
		
		/**
		 * Returns option value
		 */
		getOption: function(name) {
			return options[name];
		},
		
		/**
		 * Returns array of binded actions and their keystrokes
		 * @return {Array}
		 */
		getShortcuts: function() {
			return formatShortcut(is_mac ? mac_char_map : pc_char_map, is_mac ? '' : '+');
		},
		
		/**
		 * Show info window about Zen Coding
		 */
		showInfo: function() {
			var message = 'All textareas on this page are powered by Zen Coding project: ' +
					'a set of tools for fast HTML coding.\n\n' +
					'Available shortcuts:\n';
					
			var sh = this.getShortcuts(),
				actions = [];
				
			for (var i = 0; i < sh.length; i++) {
				actions.push(sh[i].keystroke + ' — ' + sh[i].action_name)
			}
			
			message += actions.join('\n') + '\n\n';
			message += 'More info on http://code.google.com/p/zen-coding/';
			
			alert(message);
			
		}
	}
})();