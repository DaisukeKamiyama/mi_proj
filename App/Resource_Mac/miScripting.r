#include <Carbon/Carbon.r>

#define Reserved8   reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved
#define Reserved12  Reserved8, reserved, reserved, reserved, reserved
#define Reserved13  Reserved12, reserved
#define dp_none__   noParams, "", directParamOptional, singleItem, notEnumerated, Reserved13
#define reply_none__   noReply, "", replyOptional, singleItem, notEnumerated, Reserved13
#define synonym_verb__ reply_none__, dp_none__, { }
#define plural__    "", {"", kAESpecialClassProperties, cType, "", reserved, singleItem, notEnumerated, readOnly, Reserved8, noApostrophe, notFeminine, notMasculine, plural}, {}

resource 'aete' (0, "") 
{
	//major version in BCD
	0x1,
	//minor version in BCD
	0x0,
	//language code
	english,
	//script code
	roman,
	{
		//suite name
		"Required suite",
		//suite description
		"",
		//suite ID
		kAERequiredSuite,
		//suite level
		0,
		//suite version
		0,
		{
			/* Events */
			
			//event
			"open",
			"Open the specified object(s)",
			'aevt', 'odoc',
			//reply
			reply_none__,
			//direct param
			typeAlias,
			"list of objects to open",
			directParamRequired,
			listOfItems, notEnumerated, changesState, Reserved12,
			//parameter
			{
			
			},
			
			//event
			"print",
			"Print the specified object(s)",
			'aevt', 'pdoc',
			//reply
			reply_none__,
			//direct param
			typeAlias,
			"list of objects to print",
			directParamRequired,
			listOfItems, notEnumerated, Reserved13,
			//parameter
			{
			
			},
			
			//event
			"quit",
			"Quit Navigator",
			'aevt', 'quit',
			//reply
			reply_none__,
			//direct param
			noParams,"no direct parameter required",
			directParamOptional,singleItem,notEnumerated,changesState,
			reserved,reserved,reserved,reserved,
			reserved,reserved,reserved,reserved,
			reserved,reserved,reserved,reserved,
			//parameter
			{
			
			},
			
			//event
			"run",
			"Sent to an application when it is double-clicked",
			'aevt', 'oapp',
			//reply
			reply_none__,
			//direct param
			noParams,"no direct parameter required",
			directParamOptional,singleItem,notEnumerated,changesState,
			reserved,reserved,reserved,reserved,
			reserved,reserved,reserved,reserved,
			reserved,reserved,reserved,reserved,
			//parameter
			{
			
			}
		},
		{
			/* Classes */
			
		},
		{
			/* Comparisons */
		},
		{
			/* Enumerations */
		},
		
		//suite name
		"Standard suite",
		//suite description
		"Common terms for most applications",
		//suite ID
		'CoRe',
		//suite level
		1,
		//suite version
		1,
		{
			/* Events */
			//event
			"close",
			"Close an object",
			'core', 'clos',
			//reply
			reply_none__,
			//direct param
			cObjectSpecifier,
			"the objects to close",
			directParamRequired,
			singleItem, notEnumerated, changesState, Reserved12,
			//parameter
			{
				//save option
				"saving", 'savo', 'savo',
				"specifies whether or not changes should be saved before closing",
				optional,
				singleItem, enumerated, Reserved13,
				//save file
				"in", 'kfil', typeAlias,
				"the file in which to save the object",
				optional,
				singleItem, notEnumerated, Reserved13
			},
			
			//event
			"count",
			"Return the number of elements of a particular class within an object",
			'core', 'cnte',
			//reply
			'long',
			"the number of elements",
			replyRequired, singleItem, notEnumerated, Reserved13,
			//direct param
			cObjectSpecifier,
			"the object whose elements are to be counted",
			directParamRequired,
			singleItem, notEnumerated, Reserved13,
			{
			
			},
			
			//event
			"data size",
			"Return the size in bytes of an object",
			'core', 'dsiz',
			//reply
			'long',
			"the size of the object in bytes",
			replyRequired, singleItem, notEnumerated, Reserved13,
			//direct param
			cObjectSpecifier,
			"the object whose data size is to be returned",
			directParamRequired,
			singleItem, notEnumerated, Reserved13,
			{
			
			},
			
			//event
			"delete",
			"Delete an element from an object",
			'core', 'delo',
			//reply
			reply_none__,
			//direct param
			cObjectSpecifier,
			"the element to delete",
			directParamRequired,
			singleItem, notEnumerated, changesState, Reserved12,
			{
			
			},
			
			//event
			"get",
			"Get the data for an object",
			'core', 'getd',
			//reply
			'****',
			"The data from the object",
			replyRequired, singleItem, notEnumerated, Reserved13,
			//direct param
			cObjectSpecifier,
			"the object whose data is to be returned",
			directParamRequired,
			singleItem, notEnumerated, Reserved13,
			{
			
			},
			
			//event
			"make",
			"Make a new element",
			'core', 'crel',
			//reply
			cObjectSpecifier,
			"Object specifier for the new element",
			replyRequired, singleItem, notEnumerated, Reserved13,
			//direct param
			'null',
			"",
			directParamOptional,
			singleItem, notEnumerated, changesState, Reserved12,
			//parameter
			{
				//make new
				"new", 'kocl', 'type',
				"the class of the new element",
				required,
				singleItem, notEnumerated, Reserved13,
				//at
				"at", 'insh', 'insl',
				"the location at which to insert the element",
				optional,
				singleItem, notEnumerated, Reserved13,
				//with data
				"with data", 'data', typeUnicodeText,
				"the initial data for the element",
				optional,
				singleItem, notEnumerated, Reserved13,
				//with properties
				"with properties", 'prdt', 'reco',
				"the initial values for the properties of the element",
				optional,
				singleItem, notEnumerated, Reserved13
			},
			
			//event
			"print",
			"Print the specified object(s)",
			'aevt', 'pdoc',
			//reply
			reply_none__,
			//direct param
			cObjectSpecifier,
			"Objects to print. Can be a list of files or an object specifier.",
			directParamRequired,
			singleItem, notEnumerated, Reserved13,
			//parameter
			{
			
			},
			
			//event
			"open",
			"Open the specified object(s)",
			'aevt', 'odoc',
			//reply
			reply_none__,
			//direct param
			cObjectSpecifier,
			"Objects to open. Can be a list of files or an object specifier.",
			directParamRequired,
			singleItem, notEnumerated, changesState, Reserved12,
			//parameter
			{
				//as
				"as", 'chac', typeUnicodeText,
				"the character code to be used",
				optional,
				singleItem, notEnumerated, Reserved13
			},
			
			//event
			"save",
			"save a set of objects",
			'core', 'save',
			//reply
			reply_none__,
			//direct param
			cObjectSpecifier,
			"Objects to save.",
			directParamRequired,
			singleItem, notEnumerated, Reserved13,
			//parameter
			{
				//in
				"in", 'kfil', typeAlias,
				"the file in which to save the object(s)",
				optional,
				singleItem, notEnumerated, Reserved13,
				//as
				"as", 'fltp', 'type',
				"the file type of the document in which to save the data",
				optional,
				singleItem, notEnumerated, Reserved13
			},
			
			//event
			"set",
			"Set an object’s data",
			'core', 'setd',
			//reply
			reply_none__,
			//direct param
			cObjectSpecifier,
			"the object to change",
			directParamRequired,
			singleItem, notEnumerated, changesState, Reserved12,
			//parameter
			{
				//to
				"to", 'data', '****',
				"the new value",
				required,
				singleItem, notEnumerated, Reserved13
			},
			
			//event
			"select",
			"Select the specified object",
			'misc', 'slct',
			//reply
			'null',
			"",
			replyOptional, listOfItems, notEnumerated, Reserved13,
			//direct param
			cObjectSpecifier,
			"the object to select",
			directParamOptional,singleItem,notEnumerated,changesState,
			reserved,reserved,reserved,reserved,
			reserved,reserved,reserved,reserved,
			reserved,reserved,reserved,reserved,
			//parameter
			{
				//to
				"to", 'to  ', 'obj ',
				"",
				optional,
				singleItem, notEnumerated, Reserved13
			},
			
			//event
			"exists",
			"Verify if an object exists",
			'core', 'doex',
			//reply
			'bool',
			"true if it exists, false if not",
			replyRequired, singleItem, notEnumerated, Reserved13,
			//direct param
			cObjectSpecifier,
			"the object in question",
			directParamRequired,
			singleItem, notEnumerated, Reserved13,
			//parameter
			{
			
			},
			
			//event
			"collapse",
			"collapse indexgroup",
			'core', 'coll',
			//reply
			reply_none__,
			//direct param
			cObjectSpecifier,
			"the indexgroup to be collapsed",
			directParamRequired,singleItem,notEnumerated,changesState,
			reserved,reserved,reserved,reserved,
			reserved,reserved,reserved,reserved,
			reserved,reserved,reserved,reserved,
			{
			
			},
			
			//event
			"expand",
			"expand indexgroup",
			'core', 'expa',
			//reply
			reply_none__,
			//direct param
			cObjectSpecifier,
			"the indexgroup to be expanded",
			directParamRequired,singleItem,notEnumerated,changesState,
			reserved,reserved,reserved,reserved,
			reserved,reserved,reserved,reserved,
			reserved,reserved,reserved,reserved,
			//parameter
			{
			
			}
		},
		{
			/* Classes */
			
			//class
			"application", 'capp',
			"An application program",
			//properties
			{
				//appfolder
				/*081115 2.1.8a8à»ç~îÒëŒâûÇÃÇΩÇﬂçÌèú
				"appfolder", 'fold', 0x66737300,//bug
				"folder installed the application",
				reserved, singleItem, notEnumerated, readOnly, Reserved12
				*/
			},
			//elements
			{
				'cwin', { 'indx','name','rele' },
				'docu', { 'name' },
				'idxw', { 'name' }
			},
			
			//plural
			//"applications", 'capp', plural__,
			
			//class
			"window", 'cwin',
			"A Window",
			//properties
			{
				//bounds
				"bounds", 'pbnd', 'qdrt',
				"the boundary rectangle for the window",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,
				//closable
				"closeable", 'hclb', 'bool',
				"Does the window have a close box?",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"titled", 'ptit', 'bool',
				"Does the window have a title bar?",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"index", 'pidx', 'long',
				"the index of the window in the application",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"floating", 'isfl', 'bool',
				"Does the window float?",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"modal", 'pmod', 'bool',
				"Is the window modal?",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"resizable", 'prsz', 'bool',
				"Is the window resizable?",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"zoomable", 'iszm', 'bool',
				"Is the window zoomable?",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"zoomed", 'pzum', 'bool',
				"Is the window zoomed?",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"name", 'pnam', typeUnicodeText,
				"the title of the window",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"visible", 'pvis', 'bool',
				"Is the window visible?",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"position", 'ppos', 'QDpt',
				"upper left coordinates of window",
				reserved, singleItem, notEnumerated, readOnly, Reserved12
			},
			{
			},
			//"windows", 'cwin', plural__,

			"document", 'docu',
			"A Document",
			{
				"file", 'file', typeAlias,
				"the file specification of the document",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"name", 'pnam', typeUnicodeText,
				"the file name of the document",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"modified", 'imod', 'bool',
				"Has the document been modified since the last save?",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"mode", 'pmod', typeUnicodeText,
				"mode name, for example \"HTML\" or \"TeX\" or \"C\",etc.",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"text encoding", 'pchc', typeUnicodeText,
				"Text Encoding",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"character code", 'pchc', typeUnicodeText,
				"Text Encoding (old name)",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"return code", 'prtc', 'retc',
				"return code",
				reserved, singleItem, enumerated, readWrite, Reserved12,

				"type", 'pftp', typeUnicodeText,//081115 AppleScript.cppÇ≈ÇÕUnicodeTextÇï‘ÇµÇƒÇ¢ÇÈÇÃÇ≈Å@'TEXT',
				"file type",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"creator", 'pfcr', typeUnicodeText,//081115 AppleScript.cppÇ≈ÇÕUnicodeTextÇï‘ÇµÇƒÇ¢ÇÈÇÃÇ≈Å@'TEXT',
				"file creator",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"windowindex", 'pwin', 'long',
				"the index of the window in the application",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"index", 'pidx', 'long',
				"the index of the document window",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"header", 'head', typeUnicodeText,
				"information header string",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"font", 'font', typeUnicodeText,
				"Font name",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"size", 'ptsz', 'long',
				"Size of font",
				reserved, singleItem, notEnumerated, readWrite, Reserved12
			},
			{
				'cpar', { },
				'cwor', { },
				'cha ', { },
				'cins', { },
				'csel', { }
			},
			//"documents", 'docu', plural__,

			"paragraph", 'cpar',
			"A paragraph until carige return code",
			{
				"index", 'pidx', 'long',
				"the paragraph number in the document",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"content", 'pcnt', typeUnicodeText,
				"the content text of the paragraph",
				reserved, singleItem, notEnumerated, readWrite, Reserved12
			},
			{
				'cwor', { },
				'cha ', { },
				'cins', { }
			},
			//"paragraphs", 'cpar', plural__,

			"word", 'cwor',
			"A word (an alphabet sequence) or a non-alphabet (but not space) letter",
			{
				"index", 'pidx', 'long',
				"the word index in the document",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"content", 'pcnt', typeUnicodeText,
				"the content of the word",
				reserved, singleItem, notEnumerated, readWrite, Reserved12
			},
			{
				'cha ', { },
				'cins', { }
			},
			//"words", 'cwor', plural__,

			"character", 'cha ',
			"An any kind of character ( 2-byte character is counted as one character )",
			{
				"index", 'pidx', 'long',
				"the character index in the document",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"content", 'pcnt', typeUnicodeText,
				"the content of the character",
				reserved, singleItem, notEnumerated, readWrite, Reserved12
			},
			{
				'cins', { }
			},
			//"characters", 'cha ', plural__,

			"insertion point", 'cins',
			"An insertion location between a character",
			{
				"index", 'pidx', 'long',
				"the insertion point index in the document",
				reserved, singleItem, notEnumerated, readOnly, Reserved12
			},
			{
			},
			//"insertion points", 'cins', plural__,

			"selection object", 'csel',
			"the selection or caret",
			{
				"content", 'pcnt', typeUnicodeText,//081115 AppleScript.cppÇ≈ÇÕUnicodeTextÇï‘ÇµÇƒÇ¢ÇÈÇÃÇ≈Å@'TEXT',
				"the content string of the selection object",
				reserved, singleItem, notEnumerated, readWrite, Reserved12
			},
			{
				'cwor', { },
				'cha ', { },
				'cins', { }
			},
			//"selection objects", 'csel', plural__,
			
			"indexwindow", 'idxw',
			"An index window",
			{
				"file", 'file', typeAlias,
				"the file specification of the indexwindow",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"name", 'pnam', typeUnicodeText,
				"the title of the indexwindow",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"modified", 'imod', 'bool',
				"Has the document been modified since the last save?",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"filewidth", 'fwid', 'long',
				"width of file tag",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"infowidth", 'iwid', 'long',
				"width of comment tag",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"fileorder", 'ford', 'long',
				"order of file tag",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"infoorder", 'iord', 'long',
				"order of comment tag",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"path", 'path', 'epth',
				"saving type of path",
				reserved, singleItem, enumerated, readOnly, Reserved12,

				"windowindex", 'pwin', 'long',
				"index of window",
				reserved, singleItem, notEnumerated, readOnly, Reserved12,

				"asksaving", 'asks', 'bool',
				"whether ask saving or not when closing",
				reserved, singleItem, notEnumerated, readWrite, Reserved12
			},
			{
				'idxr', { },
				'idxg', { }
			},
			//"indexwindows", 'idxw', plural__,

			"indexrecord", 'idxr',
			"index record of index window",
			{
				"file", 'file', typeAlias,
				"file information of index",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"startposition", 'spos', 'long',
				"start position of index",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"endposition", 'epos', 'long',
				"end position of index",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"paragraph", 'cpar', 'long',
				"paragraph number",
				reserved, singleItem, notEnumerated, readWrite, Reserved12,

				"comment", 'comm', typeUnicodeText,
				"comment of index",
				reserved, singleItem, notEnumerated, readWrite, Reserved12
			},
			{
			},
			//"indexrecords", 'idxr', plural__,

			"indexgroup", 'idxg',
			"group of index records",
			{
				"comment", 'comm', typeUnicodeText,
				"titile of group",
				reserved, singleItem, notEnumerated, readWrite, Reserved12
			},
			{
				'idxr', { }
			},
			//"indexgroups", 'idxg', plural__
		},
		{
			/* Comparisons */
		},
		{
			/* Enumerations */
			'savo',
			{
				"yes", 'yes ', "Save objects now",
				"no", 'no  ', "Do not save objects",
				"ask", 'ask ', "Ask the user whether to save"
			},

			'retc',
			{
				"CR", 'CR  ', "CR (Mac)",
				"CRLF", 'CRLF', "CR+LF (Windows)",
				"LF", 0x4C460000, "LF (UNIX)"
			},

			'epth',
			{
				"partial", 'part', "partial path",
				"full", 'full', "full path"
			}
		}
	}
};
