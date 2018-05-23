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

ARegExp
ê≥ãKï\åªåüçıÅ^íuä∑

*/

#pragma once

#include "../AbsBase/ABase.h"

class ADepth
{
  public:
	ADepth( ANumber &inDepth ):mDepth(inDepth) { mDepth++; }
	~ADepth() { mDepth--; }
  private:
	ANumber&	mDepth;
};

class ALetterRange 
{
  public:
	ALetterRange();
	void	Init();
	void	AddRange( const /*#731 AUTF16Char*/AUCS4Char inStart, const /*#731 AUTF16Char*/AUCS4Char inEnd );
	ABool	Match( const /*#731 AUTF16Char*/AUCS4Char inLetter ) const;
	void	SetNotMode( const ABool isNot ) { mNotMode = isNot; }
  private:
	ABool	mNotMode;
	AArray</*#731 AUTF16Char*/AUCS4Char>	mStart;
	AArray</*#731 AUTF16Char*/AUCS4Char>	mEnd;
};

const AItemCount kRangeCountMax = 100;
const AItemCount kGroupCountMax = 100;

/**
ê≥ãKï\åªåüçıÉNÉâÉX
*/
class ARegExp : public AObjectArrayItem
{
  public:
	ARegExp( AObjectArrayItem* inParent = NULL );
	ARegExp( AConstCharPtr inRE, const ABool inIgnoreCase = false, const ABool inHandleYenAsBackslash = true );//#1231
	void	InitGroup();
  private:
	void	Init();
	
  public:
	ABool	SetRE( const AText& inRE, const ABool inIgnoreCase = false, const ABool inHandleYenAsBackslash = true );//B0317 R0233 #501
	void	GetRE( AText& outRE ) const;
	void	Parse( const AText& inRE, const AIndex inStart, AIndex inEnd, AIndex &ioGroupNumber );
  private:
	ABool	ParseEscape( const AText& inRE, AIndex &ioREPos, /*#731 AUTF16Char*/AUCS4Char &outLetter );
	ABool	ParseInteger( const AText& inRE, AIndex &ioREPos, ANumber &outInt ) const;
ABool	ParseHex( const AText& inRE, AIndex &ioREPos, AUCS4Char &outHex );//#1214 AUInt16Å®AUCS4Char
	ABool	ParseRepMeta( const AText& inRE, AIndex &ioREPos, const AItemCount inLength );
	
  public:
	ABool	Match( const AText& inText, AIndex &ioTextPos, const AIndex inTextTargetEndPos );
	ABool	Match( const AText& inText );//#1231
  private:
	ABool	MatchRecursive( const AConstUCharPtr iex, const AItemCount inIexLength, AIndex &ioIexPos, 
			//#731 const AText& inText, 
			const AConstUCharPtr inTextPtr, const AItemCount inTextLength,//#731
			const AIndex inTextTargetEndPos, AIndex &ioTextPos );
	ABool	IsAlphabet( const AUChar ch ) const;
	
  public:
	void	ChangeReplaceText(  const AText& inTargetText, const AText& inText, AText &outText ) const;
	void	GetGroupRange( AIndex inGroupIndex, AIndex& outStart, AIndex& outEnd ) const;
	void	GetMatchedText( const AText& inText, AText& outMatchedText ) const;//#1231
	void	GetGroupText( const AText& inText, AIndex inGroupIndex, AText& outGroupText ) const;//#1231
	
	//#437
  public:
	static void	ConvertToREFromWildCard( const AText& inWildCard, AText& outRE );
	
  public:
	ABool	Test( AText& ioText );
	
	//óLå¯Ç»ê≥ãKï\åªÇ™ê›íËçœÇ›Ç©Ç«Ç§Ç©ÇéÊìæ
  public:
	ABool	IsREValid() const { return mREValid; }
	
  private:
	AText	mIex;
	AIndex	mGroupSpos[kGroupCountMax];
	AIndex	mGroupEpos[kGroupCountMax];
	ALetterRange	mRange[kRangeCountMax];
	AIndex	mRangeNumber;
	AIndex	mEndPosition;
	AIndex	mMatchedTextSpos;
	AIndex	mMatchedTextEpos;
	AItemCount	mGroupCount;
	AIndex	mLastMatchedGroup;
	AText	mRE;
	ANumber	mDepth;
	ABool	mCanceled;
	ABool	mREValid;//B0317
	ABool	mIgnoreCase;//R0233
	
	//ObjectèÓïÒéÊìæ
  public:
	virtual AConstCharPtr	GetClassName() const { return "ARegExp"; }
	
};
 
