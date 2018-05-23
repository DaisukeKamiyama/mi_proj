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

ASpellChecker
R0199
*/

#include "ASpellCheckerWrapper.h"
#include "SpellCheck.h"

/**
スペルチェック
*/
ABool	ASpellCheckerWrapper::CheckSpell( const AText& inUTF16Text, const AIndex inStartIndex, const AIndex inEndIndex, const ANumber inLanguage )
{
	CFStringRef	strref = NULL;
	{//#598 arrayptrの有効範囲を最小にする
		AStTextPtr	textptr(inUTF16Text,inStartIndex*sizeof(UniChar),inEndIndex*sizeof(UniChar)-inStartIndex*sizeof(UniChar));
		strref = ::CFStringCreateWithBytes(NULL,textptr.GetPtr(),textptr.GetByteCount(),kCFStringEncodingUTF16,false);
	}
	if( strref == NULL )   return true;
	CFRange	range = SpellCheck(strref,0,inLanguage);
	::CFRelease(strref);
	return (range.length==0);
}

/**
修正候補推測
*/
void	ASpellCheckerWrapper::GuessWord( const AText& inWord, ATextArray& outCandidateArray, const ANumber inLanguage ) 
{
	outCandidateArray.DeleteAll();
	AStCreateCFStringFromAText	str(inWord);
	CFArrayRef array = WordGuess(str.GetRef(),inLanguage);
	if( array != NULL )
	{
		CFIndex	arrayCount = ::CFArrayGetCount(array);
		for( CFIndex i = 0; i < arrayCount; i++ )
		{
			CFStringRef	str = (CFStringRef)::CFArrayGetValueAtIndex(array,i);
			if( str != NULL )
			{
				if( ::CFGetTypeID(str) == ::CFStringGetTypeID() )
				{
					AText	text;
					text.SetFromCFString(str);
					outCandidateArray.Add(text);
				}
			}
		}
		::CFRelease(array);
	}
}


