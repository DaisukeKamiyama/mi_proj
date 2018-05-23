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

mi_modeprefconverter
main.cpp

*/

#include "../../AbsFramework/Frame.h"
#import <Cocoa/Cocoa.h>
#include "../../AbsFramework/AbsBase/Cocoa.h"
#include "AModePrefDB.h"

int	main( int argc, const char **argv )
{
	//SIGPIPE�𖳎�
	::signal(SIGPIPE, SIG_IGN);
	
	//ABase�I�u�W�F�N�g�𐶐�����B
	ABase	base(true);
	
	//�����i���[�h�t�H���_�j�擾
	AText	argv1;
	argv1.AddCstring(argv[1]);
	
	//���[�h�t�H���_��file�Ɋi�[
	AFileAcc	file;
	file.Specify(argv1);
	
	//AModePrefDB�I�u�W�F�N�g�쐬
	AModePrefDB	modePrefDB(NULL);
	
	//MacLegacy�f�[�^�����[�h���ăo�[�W����3�`���t�@�C���ŕۑ�
	modePrefDB.LoadMacLegacyAndSave(0,file);
}

//�A�v���P�[�V������
void	GetApplicationName( AText& outName )
{
	outName.SetCstring("mi_modeprefconverter");
}

void	BackupDataForCriticalError()
{
}

void	EventTransactionPostProcess( const ABool inPeriodical )
{
}

