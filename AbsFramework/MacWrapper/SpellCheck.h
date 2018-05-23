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

CFRange	SpellCheck( CFStringRef inText, int inStart, int language );
CFArrayRef	WordGuess( CFStringRef inWord, int language );

const int kSpellCheckLanguage_English				= 0;
const int kSpellCheckLanguage_AustralianEnglish		= 1;
const int kSpellCheckLanguage_BritishEnglish		= 2;
const int kSpellCheckLanguage_CanadianEnglish		= 3;
const int kSpellCheckLanguage_German				= 4;
const int kSpellCheckLanguage_Spanish				= 5;
const int kSpellCheckLanguage_French				= 6;
const int kSpellCheckLanguage_Italian				= 7;
const int kSpellCheckLanguage_Netherland			= 8;
const int kSpellCheckLanguage_Portuguese			= 9;
const int kSpellCheckLanguage_BrasilPortuguese		= 10;
const int kSpellCheckLanguage_Svenska				= 11;


