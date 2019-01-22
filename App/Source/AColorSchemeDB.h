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

AColorSchemeDB
#889

*/

#pragma once

#include "../../AbsFramework/AbsBase/ADataBase.h"
#include "ATypes.h"

enum AColorSchemeType
{
	kColorSchemeType_CSV = 0,
	kColorSchemeType_EPF,
};

/**
カラースキームデータ
*/
class AColorSchemeDB : public ADataBase
{
  public:
	AColorSchemeDB();
	
  public:
	ABool	Load( const AText& inSchemeName );//#1316
	void	GetColor( const APrefID inPrefID, AColor& ioColor ) const;
	static AIndex	GetColorSlotIndexFromName( const AText& inName );
  private:
	void	LoadFromColorSchemeFile( const AFileAcc& inFile, const AColorSchemeType inType );
	void	LoadFromCSVFile( const AFileAcc& inFile );
	void	LoadFromEPFFile( const AFileAcc& inFile );
	AColor	ParseFromEqual_EPFFile( const AText& inText, const AIndex inPos ) const;
	
	//
  public:
	const static APrefID	kLetter					= 101;
	const static APrefID	kBackground				= 102;
	const static APrefID	kComment				= 103;
	const static APrefID	kLiteral				= 104;
	const static APrefID	kSlot0					= 105;
	const static APrefID	kSlot1					= 106;
	const static APrefID	kSlot2					= 107;
	const static APrefID	kSlot3					= 108;
	const static APrefID	kSlot4					= 109;
	const static APrefID	kSlot5					= 110;
	const static APrefID	kSlot6					= 111;
	const static APrefID	kSlot7					= 112;
	const static APrefID	kHighlight1				= 113;
	const static APrefID	kHighlight2				= 114;
	const static APrefID	kHighlight3				= 115;
	const static APrefID	kHighlight4				= 116;
	const static APrefID	kHighlight5				= 117;
	const static APrefID	kDiffOpacity			= 118;
	const static APrefID	kSelectionOpacity		= 119;
	const static APrefID	kControlCode			= 120;//#1142
	
};
