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

AThread_DocumentFileScreening
#402

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "ATextFinder.h"

#pragma mark ===========================
#pragma mark [�N���X]AThread_DocumentFileScreening
/**
AThread_DocumentFileScreening�X���b�h�I�u�W�F�N�g
*/
class AThread_DocumentFileScreening : public AThread
{
  public:
	AThread_DocumentFileScreening( AObjectArrayItem* inParent );
	
  public:
	void	SPNVI_Run( const ADocumentID inDocumentID );
  private:
	void	NVIDO_ThreadMain();
	
	//�v���f�[�^
  private:
	ADocumentID					mDocumentID;
};


#pragma mark ===========================
#pragma mark [�N���X]AThread_DocumentFileScreeningFactory
//
class AThread_DocumentFileScreeningFactory : public AThreadFactory
{
  public:
	AThread_DocumentFileScreeningFactory( AObjectArrayItem* inParent ) : mParent(inParent)
	{
	}
	AThread_DocumentFileScreening*	Create() 
	{
		return new AThread_DocumentFileScreening(mParent);
	}
  private:
	AObjectArrayItem* mParent;
};
