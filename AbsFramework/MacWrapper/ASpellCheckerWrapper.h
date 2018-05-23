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

ASpellCheckerWrapper
R0199
*/

#pragma once

#include "../AbsBase/ABase.h"

/**
スペルチェッカ Wrapper
*/
class ASpellCheckerWrapper
{
  public:
	static ABool	CheckSpell( const AText& inUTF16Text, const AIndex inStartIndex, const AIndex inEndIndex, const ANumber inLanguage );
	static void	GuessWord( const AText& inWord, ATextArray& outCandidateArray, const ANumber inLanguage );
};


