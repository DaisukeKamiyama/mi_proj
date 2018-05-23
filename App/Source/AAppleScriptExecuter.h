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

AAppleScriptExecuter

*/

#pragma once

#include "../../AbsFramework/Frame.h"

#pragma mark ===========================
#pragma mark [クラス]AAppleScriptExecuter
//
class AAppleScriptExecuter : public AThread
{
  public:
	AAppleScriptExecuter( AObjectArrayItem* inParent );
	
  public:
	void	SPI_Execute( const AFileAcc& inFile );
	void	SPI_GetSource( const AFileAcc& inFile, AText& outText );
	
  private:
	void	NVIDO_ThreadMain();
	void	GetSource();
	
	ABool	mExecuteMode;
	ABool	mGetSourceMode;
	AFileAcc	mFile;
	AText	mSourceText;
};


#pragma mark ===========================
#pragma mark [クラス]AAppleScriptExecuterFactory
//
class AAppleScriptExecuterFactory : public AThreadFactory
{
  public:
	AAppleScriptExecuterFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	AAppleScriptExecuter*	Create() 
	{
		return new AAppleScriptExecuter(mParent);
	}
	
  private:
	AObjectArrayItem* mParent;
};

