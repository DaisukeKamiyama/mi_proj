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

ACSVFile
#791

*/
/*
CSV�t�@�C���f�[�^�ǂݍ��݃N���X
RFC4180����
*/

#include "stdafx.h"

#include "ABase.h"
#include "ACSVFile.h"

#pragma mark ===========================
#pragma mark [�N���X]ACSVFile
#pragma mark ===========================

#pragma mark ---�R���X�g���N�^�E�f�X�g���N�^

/**
�R���X�g���N�^
*/
ACSVFile::ACSVFile( const AText& inCSVText, const ATextEncoding inEncoding, const AItemCount inMinColumnCount )
{
	//�t�@�C���ǂݍ���
	AText	text;
	text.SetText(inCSVText);
	
	//�e�L�X�g�G���R�[�f�B���O��UTF8,CR�ɕύX
	ABool	fallbackUsed = false;
	text.ConvertToUTF8(inEncoding,true,fallbackUsed,true);
	text.ConvertReturnCodeToCR();
	
	//�ŏ������̃J������ǉ�
	for( AIndex i = 0; i < inMinColumnCount; i++ )
	{
		mDataArray.AddNewObject();
	}
	//�e�s(row)���Ƃ̃��[�v
	AText	cell;
	AItemCount	len = text.GetItemCount();
	for( AIndex pos = 0; pos < len; )
	{
		//�e�J�������Ƃ̃��[�v
		AIndex	columnIndex = 0;
		ABool	rowEnd = false;
		while( rowEnd == false )
		{
			//1�Z�����ǂݍ���
			rowEnd = ParseCell(text,pos,cell);
			//mDataArray�ɑΉ�����J���������݂��Ă��Ȃ���΁AmDataArray�̍��ڂ�ǉ�����
			if( columnIndex >= mDataArray.GetItemCount() )
			{
				//mDataArray�̍��ځi�J�����j��ǉ�
				mDataArray.AddNewObject();
				//�J����0�̍s��-1���A��̃e�L�X�g��ǉ��i�c��1�͂��̌�ŃZ���ǉ�����j
				AItemCount	rowCount = mDataArray.GetObjectConst(0).GetItemCount();
				for( AIndex i = 0; i < rowCount-1; i++ )
				{
					mDataArray.GetObject(columnIndex).Add(GetEmptyText());
				}
			}
			//mDataArray�ɒl��ݒ�i�Z���ǉ��j
			mDataArray.GetObject(columnIndex).Add(cell);
			//���̃J������
			columnIndex++;
		}
		//�e�L�X�g���̃Z�����s�����Ă����ꍇ�AmDataArray�̎c��J�����ɂ��āA�Z����ǉ�
		for( ; columnIndex < mDataArray.GetItemCount(); columnIndex++ )
		{
			mDataArray.GetObject(columnIndex).Add(GetEmptyText());
		}
	}
	/*
	//�ŏ��̍s��parse���Acell�̐�����mDataArray�̍��ڂ�ǉ�����
	for( AIndex pos = 0; pos < len; )
	{
		//mDataArray�̍��ڂ�ǉ�
		mDataArray.AddNewObject();
		//1��cell����ǂݍ��݁B�s���I��������Abreak
		AText	cell;
		if( ParseCell(text,pos,cell) == true )
		{
			//�s���I��������break
			break;
		}
	}
	//�e�s��ǂݍ��݁AmDataArray�֊i�[
	for( AIndex pos = 0; pos < len;  )
	{
		//cell�̐��������[�v
		AItemCount	itemCount = mDataArray.GetItemCount();
		ABool	lineEnded = false;
		for( AIndex index = 0; index < itemCount; index++ )
		{
			AText	cell;
			//�s���I�����Ă��Ȃ���΁A1��cell����ǂݍ��݁B
			//�J�������s���̏ꍇ�A��ɍs���I������\�������邪�A���̏ꍇ�́A�󕶎����ǉ����邱�Ƃɂ���
			if( lineEnded == false )
			{
				lineEnded = ParseCell(text,pos,cell);
			}
			else
			{
				//�J�������s������
				//_ACError("",NULL);
			}
			//mDataArray�ɒl��ݒ�
			mDataArray.GetObject(index).Add(cell);
		}
		//�J�����������߂��Ă�����A�s�I���܂œǂݐi�߂�
		if( lineEnded == false )
		{
			//�J���������ߔ���
			//_ACError("",NULL);
			//�s�I���܂œǂݐi��
			while(lineEnded==false)
			{
				AText	cell;
				lineEnded = ParseCell(text,pos,cell);
			}
		}
	}
	*/
}

/**
1����cell��ǂݍ���
*/
ABool	ACSVFile::ParseCell( const AText& inText, AIndex& ioPos, AText& outCell ) const
{
	//�Ԃ�l������
	outCell.DeleteAll();
	//�_�u���N�I�[�e�[�V������quoted���ꂽ���[�h���ǂ����̃t���O
	ABool	quoted = false;
	//�s�����[�v
	AItemCount	len = inText.GetItemCount();
	for( ; ioPos < len; ioPos++ )
	{
		//1�����擾
		AUChar	ch = inText.Get(ioPos);
		switch(ch)
		{
			//�_�u���N�I�[�e�[�V�����̏ꍇ
		  case '\"':
			{
				if( quoted == false )
				{
					//quoted��ԂłȂ��ꍇ
					
					//quoted��Ԃֈȍ~
					quoted = true;
				}
				else
				{
					//quoted��Ԃ̏ꍇ
					
					//""�Ȃ�"�ɕϊ�
					if( ioPos+1 < len )
					{
						if( inText.Get(ioPos+1) == '\"' )
						{
							outCell.Add('\"');
							ioPos++;
							continue;
						}
					}
					//"�����Ȃ�quoted��ԉ���
					quoted = false;
				}
				break;
			}
			//�J���}
		  case ',':
			{
				if( quoted == true )
				{
					//quoted��Ԃ̏ꍇ
					
					//�J���}��ǉ�
					outCell.Add(',');
				}
				else
				{
					//quoted��ԂłȂ��ꍇ
					
					//cell�I��
					ioPos++;
					return false;
				}
				break;
			}
			//���s
		  case kUChar_LineEnd:
			{
				if( quoted == true )
				{
					//quoted��Ԃ̏ꍇ
					
					//���s��ǉ�
					outCell.Add(kUChar_LineEnd);
				}
				else
				{
					//quoted��ԂłȂ��ꍇ
					
					//�s�I��
					ioPos++;
					return true;
				}
				break;
			}
			//����ȊO�̕���
		  default:
			{
				//�����ǉ�
				outCell.Add(ch);
				break;
			}
		}
	}
	return true;
}

/**
�f�X�g���N�^
*/
ACSVFile::~ACSVFile()
{
}

#pragma mark ===========================

#pragma mark ---�f�[�^�擾

/**
�w��index�̃J�������擾
*/
void	ACSVFile::GetColumn( const AIndex inColumnIndex, ATextArray& outColumnData )
{
	//�J�������݂��Ȃ��ꍇ�͋��Ԃ�
	if( inColumnIndex >= mDataArray.GetItemCount() )
	{
		outColumnData.DeleteAll();
		return;
	}
	//�w��J�����f�[�^�擾
	outColumnData.SetFromTextArray(mDataArray.GetObjectConst(inColumnIndex));
}

/**
�w��index�̃J�������擾
*/
void	ACSVFile::GetColumn( const AIndex inColumnIndex, ANumberArray& outColumnData )
{
	//�J�������݂��Ȃ��ꍇ�͋��Ԃ�
	if( inColumnIndex >= mDataArray.GetItemCount() )
	{
		outColumnData.DeleteAll();
		return;
	}
	//�w��J�����f�[�^�擾
	outColumnData.DeleteAll();
	const ATextArray&	textArray = mDataArray.GetObjectConst(inColumnIndex);
	for( AIndex i = 0; i < textArray.GetItemCount(); i++ )
	{
		ANumber	num = textArray.GetTextConst(i).GetNumber();
		outColumnData.Add(num);
	}
}





