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

CAppleEvent


*/

#pragma once

#include "../AbsBase/ABase.h"

class CAEDesc;

#pragma mark ===========================
#pragma mark [�N���X]CAppleEventReader
/**
AppleEvent�ǂݎ��p�N���X
*/
class CAppleEventReader
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	CAppleEventReader( const AppleEvent* inAppleEvent );
	~CAppleEventReader();
  private:
	const AppleEvent*	mAppleEvent;
	
	//Param�擾
  public:
	void	GetParamDesc( const AEKeyword inAEKeyword, const DescType inDesiredType, CAEDesc& outAEDesc ) const;
	ABool	GetParamText( const AEKeyword inAEKeyword, const DescType inDesiredType, AText& outText ) const;
	ABool	GetParamFile( const AEKeyword inAEKeyword, AFileAcc& outFile ) const;
	ABool	GetParamInteger( const AEKeyword inAEKeyword, ANumber& outInteger ) const;
	ABool	GetParamEnum( const AEKeyword inAEKeyword, DescType& outEnum ) const;
	ABool	GetParamOSType( const AEKeyword inAEKeyword, OSType& outType ) const;
	void	GetPropertyText( const AEKeyword inAEKeyword, AText& outText ) const;
};

#pragma mark ===========================
#pragma mark [�N���X]CAEDescReader
/**
AEDesc�ǂݎ��p�N���X
*/
class CAEDescReader
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	CAEDescReader( const AEDesc* inAEDesc );
	~CAEDescReader();
  private:
	const AEDesc*	mDesc;
	
	//Desc���e�擾
  public:
	void	GetText( AText& outText ) const;
	void	GetFile( AFileAcc& outFile ) const;
	
	//
  public:
	void	GetKeyText( const DescType inKey, AText& outText );
};


#pragma mark ===========================
#pragma mark [�N���X]CAEDesc
/**
AEDesc�������ݗp�N���X
*/
class CAEDesc
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	CAEDesc();
	~CAEDesc();
  private:
	AEDesc	mDesc;
	
	//
  public:
	AEDesc*	GetDescPtrForWrite();
  private:
	void	ClearData();
	
	//List
  public:
	void	GetListItem( const AIndex inIndex, AFileAcc& outFile ) const;
	AItemCount	GetListItemCount() const;
	
	//�f�[�^�ݒ�
  public:
	static void	SetText( const AText& inText, AEDesc& ioDesc );
};


