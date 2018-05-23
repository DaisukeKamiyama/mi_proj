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

ABaseFunction

*/

#pragma once

#include "ABaseTypes.h"

class AObjectArrayItem;
class AArrayAbstract;

#pragma mark ===========================
#pragma mark [�N���X]ABaseFunction
/**
AbsBase�̑S�̓I�ȋ@�\�ɑ΂��鐧����s���֐��Q
*/
class ABaseFunction
{
	//���݃��C���X���b�h�����ǂ����̔���
  public:
	static ABool	InMainThread();
	
	//AObjectArrayItem�S�~������
  public:
	static void	AddToObjectArrayItemTrash( AObjectArrayItem* inObjectPtr );
	static void	DoObjectArrayItemTrashDelete();
	
	/*#344 �������[�v�[���@�\�폜�i���z���������ł͎g�p�����󋵂��܂����肦�Ȃ��̂Łj
	//�������Ǘ�
  public:
	static void	ShowLowMemoryAlertLevel1();
	static ABool	IsLevel1MemoryPoolReleased();
	static ABool	IsLevel2MemoryPoolReleased();
	static void	ReleaseMemoryPoolLevel1();
	static void	ReleaseMemoryPoolLevel2();
	*/
	
	//�G���[�����������i���ۂɂ�__FILE__��__LINE���g�p���邽�߁A�{�t�@�C���Ō��#define���g�p����B�j
  public:
	//�v���I�G���[
	static void	CriticalError( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem );
	static void	CriticalError( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, 
				const ANumber inErrorNumber, const AObjectArrayItem* inObjectArrayItem );
	//�v���IThrow
	static void	CriticalThrow( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem );
	//�G���[����
	static void	Error( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem );
	//Throw����
	static void	Throw( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem );
	//�P�Ȃ��񃍃O�̂��߁i�G���[�ł͂Ȃ��j
	static void	Info( const AConstCharPtr inFilename, const ANumber inLineNumber, const AConstCharPtr inString, const AObjectArrayItem* inObjectArrayItem );
	
	static void ShowDebugger();

	//���j�b�g�e�X�g
  public:
	static ABool	BaseComponentUnitTest();
	static void	LowMemoryTest();
	
	//���K�\���e�X�g
  public:
	static ABool	TestRegExp();
	
	//���v��� #271
  public:
	static void	HashStatics();
	
};

//�G���[����������
#define _AError(x,y) (ABaseFunction::Error(__FILE__,__LINE__,x,y))
#define _AThrow(x,y) (ABaseFunction::Throw(__FILE__,__LINE__,x,y))
#define _ACError(x,y) (ABaseFunction::CriticalError(__FILE__,__LINE__,x,y))
#define _ACErrorNum(x,n,y) (ABaseFunction::CriticalError(__FILE__,__LINE__,x,n,y))
#define _ACThrow(x,y) (ABaseFunction::CriticalThrow(__FILE__,__LINE__,x,y))
#define _AInfo(x,y) (ABaseFunction::Info(__FILE__,__LINE__,x,y))
