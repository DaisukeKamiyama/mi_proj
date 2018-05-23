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

AWindow_Fusen

*/

#pragma once

#include "../../AbsFramework/Frame.h"
#include "AView_Text.h"


#pragma mark ===========================
#pragma mark [�N���X]AWindow_Fusen
//�C���f�b�N�X�E�C���h�E�̃N���X

class AWindow_Fusen : public AWindow
{
	//�R���X�g���N�^�^�f�X�g���N�^
  public:
	AWindow_Fusen( const AViewID inTextViewID,
			const AObjectID inProjectObjectID, const AText& inRelativePath, 
			const AText& inModuleName, const ANumber inOffset );
	~AWindow_Fusen();
	
	//�tⳃf�[�^���ʎ�L�[
  private:
	AObjectID							mProjectObjectID;
	AText								mRelativePath;
	AText								mModuleName;
	ANumber								mOffset;
	
	
	//<�֘A�I�u�W�F�N�g�擾>
  public:
	AView_Text&				GetTextView();
  private:
	AViewID								mTextViewID;
	
	//<�C�x���g����>
	
	//�C�x���g�R�[���o�b�N
  private:
	ABool					EVTDO_Clicked( const AControlID inID, const AModifierKeys inModifierKeys );
	void					EVTDO_WindowBoundsChanged( const AGlobalRect& inPrevBounds, const AGlobalRect& inCurrentBounds );
	void					UpdateViewBounds();
	
	//<�C���^�[�t�F�C�X>
  public:
	void					SPI_EditFusen();
  private:
	void					NVIDO_Create( const ADocumentID inDocumentID );
	
	//�E�C���h�E����
  public:
  private:
	
	//<View>
  private:
	AControlID							mEditBoxViewControlID;
	
	//�ŏ��T�C�Y
  private:
	const static ANumber				kMinWidth = 96;
	const static ANumber				kMinHeight = 32;
	const static ANumber				kEditWindowWidthDelta = 8;
	const static ANumber				kEditWindowHeightDelta = 8;

	
	//�E�C���h�E���ݒ�^�擾
  private:
	AWindowType				NVIDO_GetWindowType() const { return kWindowType_Fusen; }
	
	//Object���擾
  public:
	AConstCharPtr			GetClassName() const { return "AWindow_Fusen"; }
};

#pragma mark ===========================
#pragma mark [�N���X]AFusenWindowFactory
class AFusenWindowFactory : public AWindowFactory
{
  public:
	AFusenWindowFactory( const AViewID inTextViewID,
			const AObjectID inProjectObjectID, const AText& inRelativePath, 
			const AText& inModuleName, const ANumber inOffset )
	{
		mTextViewID = inTextViewID;
		mProjectObjectID = inProjectObjectID;
		mRelativePath.SetText(inRelativePath);
		mModuleName.SetText(inModuleName);
		mOffset = inOffset;
	}
	AWindow_Fusen*	Create() 
	{
		return new AWindow_Fusen(mTextViewID,mProjectObjectID,mRelativePath,mModuleName,mOffset);
	}
  private:
	AViewID								mTextViewID;
	AObjectID							mProjectObjectID;
	AText								mRelativePath;
	AText								mModuleName;
	ANumber								mOffset;
};
