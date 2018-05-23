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

AEditBoxUndoer
ATextUndoer����R�s�[

*/

#include "stdafx.h"

#include "AEditBoxUndoer.h"
#include "../Frame.h"

#pragma mark ===========================
#pragma mark [�N���X]AEditBoxUndoer
#pragma mark ===========================
//Undo�����N���X
#pragma mark ---�R���X�g���N�^�^�f�X�g���N�^
AEditBoxUndoer::AEditBoxUndoer( ADocument_EditBox& inTextDocument ) : mTextDocument(inTextDocument)
,mDeletedTextArray(NULL)//#693,true)//#417
{
	/*#135
	mUndoMode = false;
	mUndoPosition = 0;
	mCanCat = false;
	//#210 ���g�p�̂��߃R�����g�A�E�gmNoContinue = false;//B0090
	*/
	Init();
}

#pragma mark ===========================

#pragma mark ---������

//#135
/**
�f�[�^������
*/
void	AEditBoxUndoer::Init()
{
	mUndoRecord_Tag.DeleteAll();
	mUndoRecord_Spos.DeleteAll();
	mUndoRecord_Epos.DeleteAll();
	mUndoRecord_DeletedTextObjectID.DeleteAll();
	mDeletedTextArray.DeleteAll();
	mUndoPosition = 0;
	mUndoMode = false;
	mCanCat = false;
}

#pragma mark ===========================

#pragma mark ---�L�^

//�A�N�V�����^�O���L�^
void	AEditBoxUndoer::RecordActionTag( const AEditBoxUndoTag inAction )
{
	//�A�N�V�����^�O�����������̂��폜
	DeleteDanglingActionTag();
	
	//�A�N�V�����^�O�ǉ�
	if( IsActionTag(inAction) == true )
	{
		AddUndoRecord(inAction,0,0,kObjectID_Invalid);//#216
	}
}

//�e�L�X�g�}�����L�^
void	AEditBoxUndoer::RecordInsertText( const ATextIndex inInsertTextIndex, const AItemCount inLength )
{
	//Undo���s���Ȃ牽�����Ȃ�
	if( mUndoMode == true )   return;
	
	//�}�����R�[�h�ǉ�
	AddUndoRecord(kEditBoxUndoTag_InsertText,inInsertTextIndex,inInsertTextIndex+inLength,kObjectID_Invalid);//#216
	
	//
	if( mCanCat == false )
	{
		mCanCat = true;
	}
	else if( /*#210 ���g�p�̂��߃R�����g�A�E�g(mNoContinue == false) &&*/
				(mUndoRecord_Tag.GetItemCount() >= 3) && (GetCurrentAction() == kEditBoxUndoTag_Typing) )
	{
		//�O���UndoRecord��InsertText�ŁA�I�������ʒu������̊J�n�����ʒu�Ɠ����Ȃ�A�܂Ƃ߂ĂP��Record�ɂ���
		AEditBoxUndoTag	prevAction = kEditBoxUndoTag_NOP;
		for( AIndex i = GetUndoRecordItemCount()-3; i >= 0; i-- )
		{
			prevAction = mUndoRecord_Tag.Get(i);
			if( prevAction >= kEditBoxUndoTag_Typing )   break;
		}
		if( prevAction == kEditBoxUndoTag_Typing )
		{
			AIndex	prevRecordIndex = GetUndoRecordItemCount()-2;
			for( ; prevRecordIndex >= 0; prevRecordIndex-- )
			{
				AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(prevRecordIndex);
				if( tag == kEditBoxUndoTag_InsertText || tag == kEditBoxUndoTag_DeleteText )   break;
			}
			if( (mUndoRecord_Tag.Get(prevRecordIndex) == kEditBoxUndoTag_InsertText) && 
						(/*#216mUndoRecord_Par*/mUndoRecord_Epos.Get(prevRecordIndex) == inInsertTextIndex) )
			{
				DeleteUndoRecord(prevRecordIndex+1,GetUndoRecordItemCount()-(prevRecordIndex+1));
				/*#216mUndoRecord_Par*/mUndoRecord_Epos.Set(GetUndoRecordItemCount()-1,inInsertTextIndex+inLength);
				mUndoPosition = GetUndoRecordItemCount();
			}
		}
	}
	//#210 ���g�p�̂��߃R�����g�A�E�g mNoContinue = false;//B0090
}

//�e�L�X�g�폜���L�^
void	AEditBoxUndoer::RecordDeleteText( const ATextIndex inStart, const ATextIndex inEnd )
{
	//Undo���s���Ȃ牽�����Ȃ�
	if( mUndoMode == true )   return;
	
	//�폜�\��e�L�X�g���擾���AmDeletedTextArray�֊i�[
	AIndex	deletedTextIndex = mDeletedTextArray.AddNewObject();
	GetTextDocument().SPI_GetText(inStart,inEnd,mDeletedTextArray.GetObject(deletedTextIndex));
	AObjectID	deletedTextObjectID = mDeletedTextArray.GetObject(deletedTextIndex).GetObjectID();
	
	//�폜���R�[�h�ǉ�
	AddUndoRecord(kEditBoxUndoTag_DeleteText,inStart,inStart,deletedTextObjectID);//#216
	
	if( false )
	{
		//
		mCanCat = true;
		//�����֎~�t���OOff
		//#210 ���g�p�̂��߃R�����g�A�E�g mNoContinue = false;//B0090
	}
	else
	{
		//�����\�t���O��Off�Ȃ�A�t���OOn�ɂ���B�������A����͌������Ȃ��B
		if( mCanCat == false )
		{
			mCanCat = true;
		}
		//#210
		//�����֎~�t���OOff���ATag��3�ȏ㑶�݂��A���݂̏�����BSKey�Ȃ猋��������s��
		else if( /*#210 ���g�p�̂��߃R�����g�A�E�g (mNoContinue == false) &&*/
					(mUndoRecord_Tag.GetItemCount() >= 3) && 
					((GetCurrentAction() == kEditBoxUndoTag_BSKey)/*||GetCurrentAction() == kEditBoxUndoTag_Typing*/) )
		{
			/*
			UndoRecord�̍\��
			[�O���ActionTag]
			[�O���InsertText]<-GetUndoRecordItemCount()-3
			[�����ActionTag]<-GetUndoRecordItemCount()-2
			[�����InsertText]
			*/
			//prevAction�ɑO���ActionTag�̓��e���擾����
			AEditBoxUndoTag	prevAction = kEditBoxUndoTag_NOP;
			for( AIndex	i = GetUndoRecordItemCount()-3; i >= 0; i-- )
			{
				prevAction = mUndoRecord_Tag.Get(i);
				if( prevAction >= kEditBoxUndoTag_Typing )   break;
			}
			//�O���ActionTag�������ActionTag�Ɠ����Ȃ猋�����葱�s
			if( prevAction == GetCurrentAction() )
			{
				//���̃��W���[���Œǉ��������R�[�h���1�O����k����InsertText/DeleteText���R�[�h�̏ꏊ��T��
				//���ꂪ������⃌�R�[�h�ƂȂ�B�C���f�b�N�X��prevRecordIndex�Ɋi�[�����B
				//���[�v��ActionTag���R�[�h���Ƃ΂����߁B
				AIndex	prevRecordIndex = GetUndoRecordItemCount()-2;
				for( ; prevRecordIndex >= 0; prevRecordIndex-- )
				{
					AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(prevRecordIndex);
					if( tag == kEditBoxUndoTag_InsertText || tag == kEditBoxUndoTag_DeleteText )   break;
				}
				//ActionTag�ɂ���ď�������
				switch(prevAction) 
				{
					/*
					#210�Ή����A���\�[�X����ڐA�������A�����̕����͕s�v�Ɏv����̂ŃR�����g�A�E�g�����B
					������A�e�X�g������Ă��Ȃ��B
					�u������I�����āA�i�㏑���Łj�������͂����ꍇ�v�̓��[�U�[�̑���Ƃ��āh���܂łƂ͕ʂ̑���h�ł���\���������̂ŁA
					�������Ȃ��ق����x�^�[���Ǝv����B
					�܂��A������ǉ�����ƁAB0090�̖��΍�Ƃ��āA�����I�����ɊO������mNoContinue��true�ɂ��鏈�����K�v�B���G�ɂȂ���Ƀ����b�g���Ȃ��B
					mNoContinue�́A���܂ŏ��false�Ŏg���Ă��Ȃ������̂ŁA����A�폜����B
					//������I�����āA�i�㏑���Łj�������͂����ꍇ
				  case undoTag_Typing:
					{
						if( (mUndoRecord_Tag.Get(prevRecordIndex) == undoTag_InsertText) &&
									(mUndoRecord_Epos.Get(prevRecordIndex) == inEnd) && 
									(mUndoRecord_Spos.Get(prevRecordIndex) <= inStart) ) 
						{
							//����ǉ��������R�[�h���폜
							DeleteUndoRecord(prevRecordIndex+1,GetUndoRecordItemCount()-(prevRecordIndex+1));
							//�����Ώۃ��R�[�h�̏I���ʒu���X�V����
							mUndoRecord_Epos.Set(GetUndoRecordItemCount()-1,
									mUndoRecord_Epos.Get(GetUndoRecordItemCount()-1)-(inEnd-inStart));
							//Undo�ʒu���X�V
							mUndoPosition = GetUndoRecordItemCount();
						}
						break;
					}
					*/
					//BSKey��A�������ꍇ
				  case kEditBoxUndoTag_BSKey:
					{
						//������⃌�R�[�h��Tag��DeleteText�ŁA���A���̊J�n�ʒu���A
						//����ǉ��������R�[�h�̏I���ʒu�ɓ������Ȃ�A�P���R�[�h�Ɍ�������
						if( (mUndoRecord_Tag.Get(prevRecordIndex) == kEditBoxUndoTag_DeleteText) &&
									(mUndoRecord_Spos.Get(prevRecordIndex) == inEnd) )
						{
							//����ǉ��������R�[�h���폜
							DeleteUndoRecord(prevRecordIndex+1,GetUndoRecordItemCount()-(prevRecordIndex+1));
							//�����Ώۃ��R�[�h�̊J�n�ʒu�ƁADeletedText���X�V����
							//����폜�e�L�X�g�擾
							AText	deletedText;
							GetTextDocument().SPI_GetText(inStart,inEnd,deletedText);
							//�����Ώۂ�DeletedText��ObjectID�擾
							AObjectID	prevRecordDeletedTextObjectID = mUndoRecord_DeletedTextObjectID.Get(prevRecordIndex);
							//DeletedText������
							mDeletedTextArray.GetObjectByID(prevRecordDeletedTextObjectID).InsertText(0,deletedText);
							//�J�n�ʒu���X�V
							mUndoRecord_Spos.Set(GetUndoRecordItemCount()-1,
									mUndoRecord_Spos.Get(GetUndoRecordItemCount()-1)-(inEnd-inStart));
							//Undo�ʒu���X�V
							mUndoPosition = GetUndoRecordItemCount();
						}
						break;
					}
				  default:
					{
						//�����Ȃ�
						break;
					}
				}
			}
		}
	}
}

#pragma mark ===========================

#pragma mark ---Undo/Redo���s

//Undo���s
void	AEditBoxUndoer::Undo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	//�A�N�V�����^�O�����������̂��폜
	DeleteDanglingActionTag();
	
	//Undo�s�Ȃ烊�^�[��
	if( mUndoPosition <= 0 )   return;
	
	mUndoMode = true;
	for( mUndoPosition--; mUndoPosition > 0; mUndoPosition-- )
	{
		//�A�N�V�����^�O�̈ʒu�܂ŗ����烋�[�v�I��
		if( IsActionTag(mUndoRecord_Tag.Get(mUndoPosition)) == true )   break;
		//mUndoPosition�ʒu�̑}���^�폜���R�[�h���A�t�]���Ď��s���A���R�[�h���t�]���ĕۑ�
		ReverseTextAction(mUndoPosition,outSelectTextIndex,outSelectTextLength);
	}
	mUndoMode = false;
}

//Redo���s
void	AEditBoxUndoer::Redo( AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	//Redo�s�Ȃ烊�^�[��
	if( mUndoPosition >= GetUndoRecordItemCount() )   return;
	
	mUndoMode = true;
	for( mUndoPosition++; mUndoPosition < GetUndoRecordItemCount(); mUndoPosition++ )
	{
		//�A�N�V�����^�O�̈ʒu�܂ŗ����烋�[�v�I��
		if( IsActionTag(mUndoRecord_Tag.Get(mUndoPosition)) == true )   break;
		//mUndoPosition�ʒu�̑}���^�폜���R�[�h���A�t�]���Ď��s���A���R�[�h���t�]���ĕۑ�
		ReverseTextAction(mUndoPosition,outSelectTextIndex,outSelectTextLength);
	}
	mUndoMode = false;
}

//�w��ʒu�̑}���^�폜���R�[�h���A�t�]���Ď��s���A���R�[�h���t�]���ĕۑ�
void	AEditBoxUndoer::ReverseTextAction( const AIndex inIndex, AIndex& outSelectTextIndex, AItemCount& outSelectTextLength )
{
	AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(inIndex);
	if( tag == kEditBoxUndoTag_InsertText )
	{
		AIndex	spos = mUndoRecord_Spos.Get(inIndex);
		AIndex	epos = /*#216mUndoRecord_Par*/mUndoRecord_Epos.Get(inIndex);
		
		//�폜�\��e�L�X�g���擾���AmDeletedTextArray�֊i�[
		AIndex	deletedTextIndex = mDeletedTextArray.AddNewObject();
		GetTextDocument().SPI_GetText(spos,epos,mDeletedTextArray.GetObject(deletedTextIndex));
		AObjectID	deletedTextObjectID = mDeletedTextArray.GetObject(deletedTextIndex).GetObjectID();
		
		//
		mUndoRecord_Tag.Set(inIndex,kEditBoxUndoTag_DeleteText);
		/*#216mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Set(inIndex,deletedTextObjectID);
		
		mCanCat = false;//#210 ==�o�O�C��
		
		GetTextDocument().SPI_DeleteText(spos,epos);
		outSelectTextIndex = spos;
		outSelectTextLength = 0;
	}
	else if( tag == kEditBoxUndoTag_DeleteText )
	{
		AIndex	spos = mUndoRecord_Spos.Get(inIndex);
		AObjectID	deletedTextObjectID = /*#216mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Get(inIndex);
		
		//
		AText	text;
		text.SetText(mDeletedTextArray.GetObject(mDeletedTextArray.GetIndexByID(deletedTextObjectID)));
		GetTextDocument().SPI_InsertText(spos,text);
		
		//
		mDeletedTextArray.Delete1Object(mDeletedTextArray.GetIndexByID(deletedTextObjectID));
		mUndoRecord_Tag.Set(inIndex,kEditBoxUndoTag_InsertText);
		/*#216mUndoRecord_Par*/mUndoRecord_Epos.Set(inIndex,spos+text.GetItemCount());
		
		mCanCat = false;//#210 ==�o�O�C��
		
		outSelectTextIndex = spos;
		outSelectTextLength = text.GetItemCount();
	}
	else
	{
		_ACError("",NULL);
	}
}

#pragma mark ===========================

#pragma mark ---���擾

//Undo���j���[�\���p������擾
ABool	AEditBoxUndoer::GetUndoMenuText( AText& outText ) const 
{
	ABool	result = false;
	outText.SetLocalizedText("EditBoxUndoAction_UndoDisabled");
	for( AIndex i = mUndoPosition-1; i >= 0; i-- ) 
	{
		AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(i);
		if( IsActionTag(tag) == true ) 
		{ 
			/*#94
			outText.SetLocalizedText("EditBoxUndoAction_",tag-kEditBoxUndoTag_Typing);
			AText	text;
			text.SetLocalizedText("EditBoxUndoAction_Undo");
			outText.AddText(text);
			*/
			AText	text;
			text.SetLocalizedText("EditBoxUndoAction_",tag-kEditBoxUndoTag_Typing);
			outText.SetLocalizedText("EditBoxUndoAction_Undo");
			outText.ReplaceParamText('0',text);
			result = true;
			break;
		}
	}
	return result;
}

//Redo���j���[�\���p������擾
ABool	AEditBoxUndoer::GetRedoMenuText( AText& outText ) const
{
	ABool	result = false;
	outText.SetLocalizedText("EditBoxUndoAction_RedoDisabled");
	for( AIndex i = mUndoPosition; i < GetUndoRecordItemCount(); i++ ) 
	{
		AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(i);
		if( IsActionTag(tag) == true ) 
		{
			/*#94
			outText.SetLocalizedText("EditBoxUndoAction_",tag-kEditBoxUndoTag_Typing);
			AText	text;
			text.SetLocalizedText("EditBoxUndoAction_Redo");
			outText.AddText(text);
			*/
			AText	text;
			text.SetLocalizedText("EditBoxUndoAction_",tag-kEditBoxUndoTag_Typing);
			outText.SetLocalizedText("EditBoxUndoAction_Redo");
			outText.ReplaceParamText('0',text);
			result = true;
			break;
		}
	}
	return result;
}

//
AEditBoxUndoTag	AEditBoxUndoer::GetCurrentAction() const
{
	for( AIndex index = mUndoPosition-1; index >= 0; index-- )
	{
		AEditBoxUndoTag	tag = mUndoRecord_Tag.Get(index);
		if( tag >= kEditBoxUndoTag_Typing )
		{
			return tag;
		}
	}
	return kEditBoxUndoTag_NOP;
}

#pragma mark ===========================

#pragma mark ---

//UndoRecord�̍Ō�ɂ���A�A�N�V�����^�O�݂̂ŁA�}���^�폜���R�[�h���Ȃ����̂��폜����
void	AEditBoxUndoer::DeleteDanglingActionTag()
{
	//UndoRecord�̍Ōォ�珇�Ɍ���
	while( GetUndoRecordItemCount() > 0 )
	{
		if( IsActionTag(mUndoRecord_Tag.Get(GetUndoRecordItemCount()-1)) == true )
		{
			//�A�N�V�����^�O�Ȃ�폜���āA���[�v�p��
			DeleteUndoRecord(GetUndoRecordItemCount()-1);
		}
		else
		{
			//�}���^�폜���R�[�h�ɂԂ������烋�[�v�I��
			break;
		}
	}
	//mUndoPosiion�X�V
	if( mUndoPosition > GetUndoRecordItemCount() )
	{
		mUndoPosition = GetUndoRecordItemCount();
	}
}

//�A�N�V�����^�O���ǂ������擾
ABool	AEditBoxUndoer::IsActionTag( const AEditBoxUndoTag inTag ) const
{
	return (inTag >= kEditBoxUndoTag_Typing);
}

//UndoRecord�ǉ�
void	AEditBoxUndoer::AddUndoRecord( const AEditBoxUndoTag inTag, const AIndex inStart, const AIndex inEnd, const AObjectID inDeletedTextObjectID )//#216const AIndex inPar )
{
	//���݂�UndoPosition�ȍ~���폜
	mUndoRecord_Tag.Delete(mUndoPosition,mUndoRecord_Tag.GetItemCount()-mUndoPosition);
	mUndoRecord_Spos.Delete(mUndoPosition,mUndoRecord_Spos.GetItemCount()-mUndoPosition);
	//#216 mUndoRecord_Par.Delete(mUndoPosition,mUndoRecord_Par.GetItemCount()-mUndoPosition);
	mUndoRecord_Epos.Delete(mUndoPosition,mUndoRecord_Epos.GetItemCount()-mUndoPosition);//#216
	mUndoRecord_DeletedTextObjectID.Delete(mUndoPosition,mUndoRecord_DeletedTextObjectID.GetItemCount()-mUndoPosition);//#216
	
	//�ǉ�
	mUndoRecord_Tag.Add(inTag);
	mUndoRecord_Spos.Add(inStart);
	//#216 mUndoRecord_Par.Add(inPar);
	mUndoRecord_Epos.Add(inEnd);//#216
	mUndoRecord_DeletedTextObjectID.Add(inDeletedTextObjectID);//#216
	
	//UndoPosition�X�V
	mUndoPosition = GetUndoRecordItemCount();
}

//UndoRecord�폜
void	AEditBoxUndoer::DeleteUndoRecord( const AIndex inIndex )
{
	//�폜���R�[�h�̏ꍇ�͕ۑ������e�L�X�g���폜
	if( mUndoRecord_Tag.Get(inIndex) == kEditBoxUndoTag_DeleteText )
	{
		mDeletedTextArray.Delete1Object(mDeletedTextArray.GetIndexByID(/*#216mUndoRecord_Par*/mUndoRecord_DeletedTextObjectID.Get(inIndex)));
	}
	
	//�폜
	mUndoRecord_Tag.Delete1(inIndex);
	mUndoRecord_Spos.Delete1(inIndex);
	//#216 mUndoRecord_Par.Delete1(inIndex);
	mUndoRecord_Epos.Delete1(inIndex);//#216
	mUndoRecord_DeletedTextObjectID.Delete1(inIndex);//#216
}

//UndoRecord�폜
void	AEditBoxUndoer::DeleteUndoRecord( const AIndex inIndex, const AItemCount inCount )
{
	for( AIndex i = 0; i < inCount; i++ )
	{
		DeleteUndoRecord(inIndex);
	}
}

