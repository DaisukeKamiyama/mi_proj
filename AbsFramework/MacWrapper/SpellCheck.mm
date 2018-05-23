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
/*

SpellCheck
R0199
*/

#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>

#include "SpellCheck.h"

CFRange	SpellCheck( CFStringRef inText, int inStart, int language )
{
	//#688 NSRange	range = {0,0};
	//#688 NSAutoreleasePool*	pool = [[NSAutoreleasePool alloc] init];
	//http://www.loc.gov/standards/iso639-2/php/English_list.php
	//http://ja.wikipedia.org/wiki/ISO_3166-1
	switch(language)
	{
	  case kSpellCheckLanguage_AustralianEnglish:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"en_AU"];
			break;
		}
	  case kSpellCheckLanguage_BritishEnglish:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"en_GB"];
			break;
		}
	  case kSpellCheckLanguage_CanadianEnglish:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"en_CA"];
			break;
		}
	  case kSpellCheckLanguage_German:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"de"];
			break;
		}
	  case kSpellCheckLanguage_Spanish:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"es"];
			break;
		}
	  case kSpellCheckLanguage_French:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"fr"];
			break;
		}
	  case kSpellCheckLanguage_Italian:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"it"];
			break;
		}
	  case kSpellCheckLanguage_Netherland:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"nl"];
			break;
		}
	  case kSpellCheckLanguage_Portuguese:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"pt"];
			break;
		}
	  case kSpellCheckLanguage_BrasilPortuguese:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"pt_BR"];
			break;
		}
	  case kSpellCheckLanguage_Svenska:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"sv"];
			break;
		}
	  case kSpellCheckLanguage_English:
	  default:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"en"];
			break;
		}
	}
	NSRange	range = [[NSSpellChecker sharedSpellChecker] checkSpellingOfString:(NSString*)inText startingAt:inStart];
	//#688 [pool release];
	CFRange	cfrange = {0};
	cfrange.location = range.location;
	cfrange.length = range.length;
	return cfrange;//#688 *(CFRange*)&range;
}

CFArrayRef	WordGuess( CFStringRef inWord, int language )
{
	//#688 NSArray*	array;
	//#688 NSAutoreleasePool*	pool = [[NSAutoreleasePool alloc] init];
	//http://www.loc.gov/standards/iso639-2/php/English_list.php
	//http://ja.wikipedia.org/wiki/ISO_3166-1
	switch(language)
	{
	  case kSpellCheckLanguage_AustralianEnglish:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"en_AU"];
			break;
		}
	  case kSpellCheckLanguage_BritishEnglish:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"en_GB"];
			break;
		}
	  case kSpellCheckLanguage_CanadianEnglish:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"en_CA"];
			break;
		}
	  case kSpellCheckLanguage_German:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"de"];
			break;
		}
	  case kSpellCheckLanguage_Spanish:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"es"];
			break;
		}
	  case kSpellCheckLanguage_French:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"fr"];
			break;
		}
	  case kSpellCheckLanguage_Italian:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"it"];
			break;
		}
	  case kSpellCheckLanguage_Netherland:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"nl"];
			break;
		}
	  case kSpellCheckLanguage_Portuguese:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"pt"];
			break;
		}
	  case kSpellCheckLanguage_BrasilPortuguese:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"pt_BR"];
			break;
		}
	  case kSpellCheckLanguage_Svenska:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"sv"];
			break;
		}
	  case kSpellCheckLanguage_English:
	  default:
		{
			[[NSSpellChecker sharedSpellChecker] setLanguage:@"en"];
			break;
		}
	}
	NSArray*	array = [[[NSSpellChecker sharedSpellChecker] guessesForWord:(NSString *)inWord] retain];
	//#688 [pool release];
	return (CFArrayRef)array;
}

