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

ABaseUnitTest

*/

#include "stdafx.h"

#include "ABase.h"
#include "../Wrapper.h"

#pragma mark ===========================
#pragma mark ---���j�b�g�e�X�g�p���[�e�B���e�B

//extern const char* kAllSJISChars;

void	MakeAllUnicodeCharText( AText& outText )
{
	outText.DeleteAll();
	for( AUTF16Char	ch = 0x0; ; ch++ )
	{
		if( ch == 0xD800 )   ch = 0xE000;//�T���Q�[�g�y�A�͂Ƃ΂�
		outText.Add((ch&0xFF));
		outText.Add((ch&0xFF00)/0x100);
		if( (ch%0x10)==0x0F )
		{
			outText.Add(13);
			outText.Add(0);
		}
		if( ch == 0xFFFF )   break;
	}
	ABool	fallback;
	if( ATextEncodingWrapper::ConvertToUTF8(outText,ATextEncodingWrapper::GetUTF16LETextEncoding(),false,fallback,false) == false )   _ACThrow("",NULL);
}

void	MakeAllSJISCharText( AText& outSJISText )
{
	outSJISText.DeleteAll();
	//B0000 TestPattern�t�@�C���ɕύX outSJISText.SetCstring(kAllSJISChars);
	AFileAcc	testfolder;
	AFileWrapper::GetLocalizedResourceFile("TestPatterns",testfolder);
	AFileAcc	file;
	file.SpecifyChild(testfolder,"SJISChars");
	file.ReadTo(outSJISText);
	//outSJISText.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
	/*for( AUChar ch1 = 0x81; ; ch1++ )
	{
		if( ch1 == 0xA0 )   ch1 = 0xE0;
		for( AUChar ch2 = 0x40; ; ch2++ )
		{
			outSJISText.Add(ch1);
			outSJISText.Add(ch2);
			if( ch2 == 0xFF )   break;
		}
		if( ch1 == 0xFC )   break;
	}*/
}

#pragma mark ===========================
#pragma mark ---���j�b�g�e�X�g

void	ABaseFunction::LowMemoryTest()
{
	/*#344 �������[�v�[���@�\�폜�i���z���������ł͎g�p�����󋵂��܂����肦�Ȃ��̂Łj
	ABool	ok = false;
	_AInfo("Low Memory Test",NULL);
	try
	{
		if( ABaseFunction::IsLevel1MemoryPoolReleased() == true )   _ACThrow("",NULL);
		if( ABaseFunction::IsLevel2MemoryPoolReleased() == true )   _ACThrow("",NULL);
		
		_AInfo("Allocate memory till under 1MB.",NULL);
		try
		{
			for( long i = 0; i < 1000000000; i++ )
			{
				void*	ptr = malloc(kByteCount_MemoryPoolLevel1Size);
				if( ptr == NULL )   break;
			}
		}
		catch(...)
		{
		}
		//���̏�ԂŎc��1MB�����ƂȂ��Ă���
		
		AArray<char>	test;
		test.Reserve(0,kByteCount_MemoryPoolLevel1Size-512);//�����Ń��x��1��������͂���1MB����A1MB�m�ۂȂ̂ŁA�c��1MB����
		//���x��1�̂݉���̊m�F
		if( ABaseFunction::IsLevel1MemoryPoolReleased() == false )   _ACThrow("",NULL);
		if( ABaseFunction::IsLevel2MemoryPoolReleased() == true )   _ACThrow("",NULL);
		_AInfo("Level 1 memory pool release OK.",NULL);
		//test�擾OK�̊m�F
		*for( long i = 0; i < kByteCount_MemoryPoolLevel1Size/2; i++ )
		{
			test.Add('a');
		}
		if( test.GetItemCount() != kByteCount_MemoryPoolLevel1Size/2 )   _ACThrow("",NULL);*
		
		try
		{
			AArray<char>	test;
			test.Reserve(0,kByteCount_MemoryPoolLevel1Size-512);//�����Ń��x��2�������Athrow�����͂�
			_ACError("",NULL);
		}
		catch(...)
		{
			//���x��1,2����̊m�F
			if( ABaseFunction::IsLevel1MemoryPoolReleased() == false )   _ACThrow("",NULL);
			if( ABaseFunction::IsLevel2MemoryPoolReleased() == false )   _ACThrow("",NULL);
			ok = true;
			_AInfo("Level 2 memory pool release OK.",NULL);
		}
		if( ok == true )
		{
			_AInfo("LowMemoryTest is OK!",NULL);
		}
	}
	catch(...)
	{
	}
	*/
}

ABool	ABaseFunction::BaseComponentUnitTest()
{
	/*malloc�����x�v���p�R�[�h
	//
	AText	text;
	{
		AText	array;
		//
		ADateTime	t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex i = 0; i < 10000000; i++ )
		{
			array.Add('a');
		}
		text.AddFormattedCstring("AText\nAdd 10M chars:%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
		//
		t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex i = 0; i < 100; i++ )
		{
			array.Insert1(0,'a');
		}
		text.AddFormattedCstring("Insert 100 chars at the top:%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
		//
		t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex i = 0; i < 100; i++ )
		{
			array.Delete1(0);
		}
		text.AddFormattedCstring("Delete 100 chars at the top:%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
	}
	{
		AArray<AIndex>	array;
		//
		ADateTime	t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex i = 0; i < 10000000; i++ )
		{
			array.Add(i);
		}
		text.AddFormattedCstring("AArray<AIndex>\nAdd 10M items:%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
		//
		t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex i = 0; i < 100; i++ )
		{
			array.Insert1(0,i);
		}
		text.AddFormattedCstring("Insert 100 items at the top:%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
		//
		t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex i = 0; i < 100; i++ )
		{
			array.Delete1(0);
		}
		text.AddFormattedCstring("Delete 100 items at the top:%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
		//
		t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex j = 0; j < 100; j++ )
		{
			AItemCount	itemCount = array.GetItemCount();
			AStArrayPtr<AIndex>	arrayptr(array,0,itemCount);
			AIndex*	p = arrayptr.GetPtr();
			for( AIndex i = 0; i < itemCount; i++ )
			{
				if( p[i] == 9999999 )
				{
					break;
				}
			}
		}
		text.AddFormattedCstring("Find 9999999th item * 100 times (using ptr):%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
		//
		t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex j = 0; j < 100; j++ )
		{
			AItemCount	itemCount = array.GetItemCount();
			AStArrayPtr<AIndex>	arrayptr(array,0,itemCount);
			AIndex*	p = arrayptr.GetPtr();
			for( AIndex i = 0; i < itemCount; i++ )
			{
				p[i]++;
			}
		}
		text.AddFormattedCstring("Increment 10M items * 100 times (using ptr):%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
		//
		t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex j = 0; j < 10; j++ )
		{
			AItemCount	itemCount = array.GetItemCount();
			for( AIndex i = 0; i < itemCount; i++ )
			{
				array.Set(i,array.Get(i)+1);
			}
		}
		text.AddFormattedCstring("Increment 10M items * 10 times (using Get()/Set()):%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
	}
	{
		AHashArray<AIndex>	array;
		//
		array.MakeHash(10000000);
		//
		ADateTime	t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex i = 0; i < 10000000; i++ )
		{
			array.Add(i);
		}
		text.AddFormattedCstring("AHashArray<AIndex>\nAdd 10M items:%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
		//
		t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex i = 0; i < 10; i++ )
		{
			array.Insert1(0,i);
		}
		text.AddFormattedCstring("Insert 10 items at the top:%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
		//
		t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex i = 0; i < 10; i++ )
		{
			array.Delete1(0);
		}
		text.AddFormattedCstring("Delete 10 items at the top:%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
		//
		t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex j = 0; j < 10000000; j++ )
		{
			array.Find(9999999);
		}
		text.AddFormattedCstring("Find 9999999th item * 100 times (using hash):%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
		//
		t0 = ADateTimeWrapper::GetCurrentTime();
		for( AIndex j = 0; j < 1; j++ )
		{
			for( AIndex i = 0; i < 10000; i++ )
			{
				array.Set(i,array.Get(i)+1);
			}
		}
		text.AddFormattedCstring("Increment 10000 items (using Get()/Set()):%f (sec)\n",ADateTimeWrapper::GetCurrentTime()-t0);
	}
	//
	text.OutputToStderr();
	*/
	ABool	result = true;
	try
	{
		//
		{
			AUniqID	uniqID = {0};
			AUniqIDArray	uniqIDArray;
			//
			uniqIDArray.InsertItems(0,100);
			if( uniqIDArray.Get(0).val != 1 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(99).val != 100 )   _ACThrow("",NULL);
			uniqID.val = 1;
			if( uniqIDArray.Find(uniqID) != 0 )   _ACThrow("",NULL);
			uniqID.val = 100;
			if( uniqIDArray.Find(uniqID) != 99 )   _ACThrow("",NULL);
			//
			uniqIDArray.DeleteItems(50,2);//UniqID{51}{52}�����g�p��ԂɂȂ�
			if( uniqIDArray.Get(0).val != 1 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(97).val != 100 )   _ACThrow("",NULL);
			uniqID.val = 1;
			if( uniqIDArray.Find(uniqID) != 0 )   _ACThrow("",NULL);
			uniqID.val = 100;
			if( uniqIDArray.Find(uniqID) != 97 )   _ACThrow("",NULL);
			//
			if( uniqIDArray.Get(49).val != 50 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(50).val != 53 )   _ACThrow("",NULL);
			uniqID.val = 50;
			if( uniqIDArray.Find(uniqID) != 49 )   _ACThrow("",NULL);
			uniqID.val = 53;
			if( uniqIDArray.Find(uniqID) != 50 )   _ACThrow("",NULL);
			//
			uniqIDArray.InsertItems(0,3);//{52}{51}���Ďg�p���A{101}��V���Ɏg��
			if( uniqIDArray.Get(0).val != 52 )   _ACThrow("",NULL);
			uniqID.val = 52;
			if( uniqIDArray.Find(uniqID) != 0 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(1).val != 51 )   _ACThrow("",NULL);
			uniqID.val = 51;
			if( uniqIDArray.Find(uniqID) != 1 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(2).val != 101 )   _ACThrow("",NULL);
			uniqID.val = 101;
			if( uniqIDArray.Find(uniqID) != 2 )   _ACThrow("",NULL);
			//
			uniqIDArray.Unuse1Item(70);//{68}�����g�p�ɂȂ�
			if( uniqIDArray.Get(70).val != -2 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(0).val != 52 )   _ACThrow("",NULL);
			uniqID.val = 52;
			if( uniqIDArray.Find(uniqID) != 0 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(1).val != 51 )   _ACThrow("",NULL);
			uniqID.val = 51;
			if( uniqIDArray.Find(uniqID) != 1 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(2).val != 101 )   _ACThrow("",NULL);
			uniqID.val = 101;
			if( uniqIDArray.Find(uniqID) != 2 )   _ACThrow("",NULL);
			//
			AIndex	index = uniqIDArray.ReuseUnusedItem();
			if( index != 70 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(70).val != 70 )   _ACThrow("",NULL);
			uniqID.val = 70;
			if( uniqIDArray.Find(uniqID) != 70 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(0).val != 52 )   _ACThrow("",NULL);
			uniqID.val = 52;
			if( uniqIDArray.Find(uniqID) != 0 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(1).val != 51 )   _ACThrow("",NULL);
			uniqID.val = 51;
			if( uniqIDArray.Find(uniqID) != 1 )   _ACThrow("",NULL);
			if( uniqIDArray.Get(2).val != 101 )   _ACThrow("",NULL);
			uniqID.val = 101;
			if( uniqIDArray.Find(uniqID) != 2 )   _ACThrow("",NULL);
			//
			index = uniqIDArray.ReuseUnusedItem();
			if( index != kIndex_Invalid )   _ACThrow("",NULL);
		}
		{
			AArray<AIndex>	array1;
			array1.Add(1);
			array1.Add(2);
			AArray<AIndex>	array2;
			array2.Add(3);
			array2.Add(4);
			array2.Add(5);
			array1.Insert(1,array2);
			if( array1.Get(0) != 1 )   _ACThrow("",NULL);
			if( array1.Get(1) != 3 )   _ACThrow("",NULL);
			if( array1.Get(2) != 4 )   _ACThrow("",NULL);
			if( array1.Get(3) != 5 )   _ACThrow("",NULL);
			if( array1.Get(4) != 2 )   _ACThrow("",NULL);
		}
		/*
		for( long i = 0; i < 1000000; i++ )
		{
		try
		{
		ATest*	ptest = new ATest();
		if( ptest == NULL )
		{
		fprintf(stderr,"NULL:%d ",i);
		break;
		}
		}
		catch(...)
		{
		fprintf(stderr,"catch:%d ",i);
		break;
		}
		}*/
		//AHashArray<ANumber>
		_AInfo("ClassTest: AHashArray<ANumber>",NULL);
		{
			AHashArray<ANumber>	a;
			{
				a.Add(123);
				a.Add(456789);
				if( a.Find(123) != 0 )   _ACThrow("",NULL);
				if( a.Find(456789) != 1 )   _ACThrow("",NULL);
				if( a.Find(999) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Insert1(0,999999);
				if( a.Find(999999) != 0 )   _ACThrow("",NULL);
				if( a.Find(123) != 1 )   _ACThrow("",NULL);
				if( a.Find(456789) != 2 )   _ACThrow("",NULL);
				if( a.Find(999) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Delete1(0);
				if( a.Find(123) != 0 )   _ACThrow("",NULL);
				if( a.Find(456789) != 1 )   _ACThrow("",NULL);
				if( a.Find(999) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Set(0,-333333333);
				if( a.Find(-333333333) != 0 )   _ACThrow("",NULL);
				if( a.Find(456789) != 1 )   _ACThrow("",NULL);
				if( a.Find(123) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Delete1(0);
				a.Delete1(0);
				
				AItemCount	count = 9999999;
#if IMPLEMENTATION_FOR_WINDOWS
				//Windows��malloc�Ɏ��Ԃ�������̂�
				count = 9999;
#endif
				for( ANumber i = 0; i < count; i++ )
						{
							a.Add(i);
				}
				ABool	b = true;
				for( ANumber i = 0; i < count; i++ )
						{
							if( a.Find(i) != i )   b = false;
				}
				if( b == false )   _ACThrow("",NULL);
			}
			a.DeleteAll();
			{
				a.Add(123);
				a.Add(456789);
				if( a.Find(123) != 0 )   _ACThrow("",NULL);
				if( a.Find(456789) != 1 )   _ACThrow("",NULL);
				if( a.Find(999) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Insert1(0,999999);
				if( a.Find(999999) != 0 )   _ACThrow("",NULL);
				if( a.Find(123) != 1 )   _ACThrow("",NULL);
				if( a.Find(456789) != 2 )   _ACThrow("",NULL);
				if( a.Find(999) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Delete1(0);
				if( a.Find(123) != 0 )   _ACThrow("",NULL);
				if( a.Find(456789) != 1 )   _ACThrow("",NULL);
				if( a.Find(999) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Set(0,-333333333);
				if( a.Find(-333333333) != 0 )   _ACThrow("",NULL);
				if( a.Find(456789) != 1 )   _ACThrow("",NULL);
				if( a.Find(123) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Delete1(0);
				a.Delete1(0);
				
				AItemCount	count = 9999999;
#if IMPLEMENTATION_FOR_WINDOWS
				//Windows��malloc�Ɏ��Ԃ�������̂�
				count = 9999;
#endif
				for( ANumber i = 0; i < count; i++ )
				{
					a.Add(i);
				}
				ABool	b = true;
				for( ANumber i = 0; i < count; i++ )
				{
					if( a.Find(i) != i )   b = false;
				}
				if( b == false )   _ACThrow("",NULL);
			}
			a.DeleteAll();
			a.MakeHash(1000);
			{
				a.Add(123);
				a.Add(456789);
				if( a.Find(123) != 0 )   _ACThrow("",NULL);
				if( a.Find(456789) != 1 )   _ACThrow("",NULL);
				if( a.Find(999) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Insert1(0,999999);
				if( a.Find(999999) != 0 )   _ACThrow("",NULL);
				if( a.Find(123) != 1 )   _ACThrow("",NULL);
				if( a.Find(456789) != 2 )   _ACThrow("",NULL);
				if( a.Find(999) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Delete1(0);
				if( a.Find(123) != 0 )   _ACThrow("",NULL);
				if( a.Find(456789) != 1 )   _ACThrow("",NULL);
				if( a.Find(999) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Set(0,-333333333);
				if( a.Find(-333333333) != 0 )   _ACThrow("",NULL);
				if( a.Find(456789) != 1 )   _ACThrow("",NULL);
				if( a.Find(123) != kIndex_Invalid )   _ACThrow("",NULL);
				a.Delete1(0);
				a.Delete1(0);
				
				AItemCount	count = 9999999;
#if IMPLEMENTATION_FOR_WINDOWS
				//Windows��malloc�Ɏ��Ԃ�������̂�
				count = 9999;
#endif
				for( ANumber i = 0; i < count; i++ )
				{
					a.Add(i);
				}
				ABool	b = true;
				for( ANumber i = 0; i < count; i++ )
				{
					if( a.Find(i) != i )   b = false;
				}
				if( b == false )   _ACThrow("",NULL);
			}
		}
		//ATextArray
		{
			ATextArray	a;
			{
				AText	text1("a");
				AText	text2("bc");
				AText	text3("def");
				a.Add(text1);
				a.Add(text2);
				a.Add(text3);
				a.Add("test");
				a.Add("1234567890");
				AText	text;
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare(text3) == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(4,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				a.Delete1(2);
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				AText	text4("test");
				AText	text5("1234567890");
				if( a.Find(text4) != 2 )   _ACThrow("",NULL);
				if( a.Find(text5) != 3 )   _ACThrow("",NULL);
				a.Insert1(2,text3);
				a.Get(0,text);
				a.Set(0,text);
				a.Get(4,text);
				a.Set(4,text);
				a.Get(0,text);
				a.Delete1(0);
				a.Insert1(0,text);
				a.Get(4,text);
				a.Delete1(4);
				a.Insert1(4,text);
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare(text3) == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(4,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				a.Delete1(3);
				a.Insert1(4,text);
				a.Get(4,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				AText	t2("abcde1234567890fghijk");
				if( a.Compare(3,t2,5,10) == false )   _ACThrow("",NULL);
				AText	t3("1234567890");
				if( a.Compare(3,t3) == false )   _ACThrow("",NULL);
				text.SetCstring("CTextHandler.cp");
				a.Set(0,text);
				a.Set(1,text);
				AText	text_;
				text_.SetCstring("CTextHandler.cp ");
				a.Set(0,text_);
				if( a.Find(text) != 1 )   _ACThrow("",NULL);
			}
			
			a.DeleteAll();
			
			{
				AText	text1("a");
				AText	text2("ab");
				AText	text3("abc");
				AText	text4("abcd");
				AText	text5("abcde");
				AText	text6("abcdef");
				AText	text7("abcdefghijklmlopqraaaaaaaaaaaaaaaaaaaaaa");
				a.Add(text1);
				a.Add(text2);
				a.Add(text3);
				a.Add(text4);
				a.Add(text5);
				a.Add(text6);
				a.Add(text7);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != 4 )   _ACThrow("",NULL);
				if( a.Find(text6) != 5 )   _ACThrow("",NULL);
				if( a.Find(text7) != 6 )   _ACThrow("",NULL);
				a.Delete1(5);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != 4 )   _ACThrow("",NULL);
				if( a.Find(text6) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text7) != 5 )   _ACThrow("",NULL);
				a.Delete1(4);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text6) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text7) != 4 )   _ACThrow("",NULL);
				AText	text;
				text.SetCstring("CTextHandler.cp");
				a.Set(0,text);
				a.Set(1,text);
				AText	text_;
				text_.SetCstring("CTextHandler.cp ");
				a.Set(0,text_);
				if( a.Find(text) != 1 )   _ACThrow("",NULL);
				a.Insert1(0,text_);
			}
		}
		//AHashTextArray
		_AInfo("ClassTest: AHashTextArray",NULL);
		{
			AHashTextArray	a;
			{
				AText	text1("a");
				AText	text2("bc");
				AText	text3("def");
				a.Add(text1);
				a.Add(text2);
				a.Add(text3);
				a.Add("test");
				a.Add("1234567890");
				AText	text;
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare(text3) == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(4,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				a.Delete1(2);
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				AText	text4("test");
				AText	text5("1234567890");
				if( a.Find(text4) != 2 )   _ACThrow("",NULL);
				if( a.Find(text5) != 3 )   _ACThrow("",NULL);
				a.Insert1(2,text3);
				a.Get(0,text);
				a.Set(0,text);
				a.Get(4,text);
				a.Set(4,text);
				a.Get(0,text);
				a.Delete1(0);
				a.Insert1(0,text);
				a.Get(4,text);
				a.Delete1(4);
				a.Insert1(4,text);
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare(text3) == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(4,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				a.Delete1(3);
				a.Insert1(4,text);
				a.Get(4,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				text.SetCstring("CTextHandler.cp");
				a.Set(0,text);
				a.Set(1,text);
				AText	text_;
				text_.SetCstring("CTextHandler.cp ");
				a.Set(0,text_);
				if( a.Find(text) != 1 )   _ACThrow("",NULL);
			}
			
			a.DeleteAll();
			
			{
				AText	text1("a");
				AText	text2("ab");
				AText	text3("abc");
				AText	text4("abcd");
				AText	text5("abcde");
				AText	text6("abcdef");
				AText	text7("abcdefghijklmlopqraaaaaaaaaaaaaaaaaaaaaa");
				a.Add(text1);
				a.Add(text2);
				a.Add(text3);
				a.Add(text4);
				a.Add(text5);
				a.Add(text6);
				a.Add(text7);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != 4 )   _ACThrow("",NULL);
				if( a.Find(text6) != 5 )   _ACThrow("",NULL);
				if( a.Find(text7) != 6 )   _ACThrow("",NULL);
				a.Delete1(5);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != 4 )   _ACThrow("",NULL);
				if( a.Find(text6) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text7) != 5 )   _ACThrow("",NULL);
				a.Delete1(4);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text6) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text7) != 4 )   _ACThrow("",NULL);
				AText	text;
				text.SetCstring("CTextHandler.cp");
				a.Set(0,text);
				a.Set(1,text);
				AText	text_;
				text_.SetCstring("CTextHandler.cp ");
				a.Set(0,text_);
				if( a.Find(text) != 1 )   _ACThrow("",NULL);
				a.Insert1(0,text_);
			}
		}
		//ATextArray #348
		{
			ATextArray	a;
			{
				AText	text1("a");
				AText	text2("bc");
				AText	text3("def");
				a.Add(text1);
				a.Add(text2);
				a.Add(text3);
				a.Add("test");
				a.Add("1234567890");
				AText	text;
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare(text3) == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(4,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				a.Delete1(2);
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				//if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				//if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				AText	text4("test");
				AText	text5("1234567890");
				//if( a.Find(text4) != 2 )   _ACThrow("",NULL);
				//if( a.Find(text5) != 3 )   _ACThrow("",NULL);
				a.Insert1(2,text3);
				a.Get(0,text);
				a.Set(0,text);
				a.Get(4,text);
				a.Set(4,text);
				a.Get(0,text);
				a.Delete1(0);
				a.Insert1(0,text);
				a.Get(4,text);
				a.Delete1(4);
				a.Insert1(4,text);
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare(text3) == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(4,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				a.Delete1(3);
				a.Insert1(4,text);
				a.Get(4,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				text.SetCstring("CTextHandler.cp");
				a.Set(0,text);
				a.Set(1,text);
				AText	text_;
				text_.SetCstring("CTextHandler.cp ");
				a.Set(0,text_);
				//if( a.Find(text) != 1 )   _ACThrow("",NULL);
			}
			
			a.DeleteAll();
			
			{
				AText	text1("a");
				AText	text2("ab");
				AText	text3("abc");
				AText	text4("abcd");
				AText	text5("abcde");
				AText	text6("abcdef");
				AText	text7("abcdefghijklmlopqraaaaaaaaaaaaaaaaaaaaaa");
				a.Add(text1);
				a.Add(text2);
				a.Add(text3);
				a.Add(text4);
				a.Add(text5);
				a.Add(text6);
				a.Add(text7);
				/*
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != 4 )   _ACThrow("",NULL);
				if( a.Find(text6) != 5 )   _ACThrow("",NULL);
				if( a.Find(text7) != 6 )   _ACThrow("",NULL);
				*/
				a.Delete1(5);
				/*
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != 4 )   _ACThrow("",NULL);
				if( a.Find(text6) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text7) != 5 )   _ACThrow("",NULL);
				*/
				a.Delete1(4);
				/*
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text6) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text7) != 4 )   _ACThrow("",NULL);
				*/
				AText	text;
				text.SetCstring("CTextHandler.cp");
				a.Set(0,text);
				a.Set(1,text);
				AText	text_;
				text_.SetCstring("CTextHandler.cp ");
				a.Set(0,text_);
				//if( a.Find(text) != 1 )   _ACThrow("",NULL);
				a.Insert1(0,text_);
			}
		}
		//AHashTextArray #348
		{
			AHashTextArray	a;
			{
				AText	text1("a");
				AText	text2("bc");
				AText	text3("def");
				a.Add(text1);
				a.Add(text2);
				a.Add(text3);
				a.Add("test");
				a.Add("1234567890");
				AText	text;
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare(text3) == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(4,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				a.Delete1(2);
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				AText	text4("test");
				AText	text5("1234567890");
				if( a.Find(text4) != 2 )   _ACThrow("",NULL);
				if( a.Find(text5) != 3 )   _ACThrow("",NULL);
				a.Insert1(2,text3);
				a.Get(0,text);
				a.Set(0,text);
				a.Get(4,text);
				a.Set(4,text);
				a.Get(0,text);
				a.Delete1(0);
				a.Insert1(0,text);
				a.Get(4,text);
				a.Delete1(4);
				a.Insert1(4,text);
				a.Get(0,text);
				if( text.Compare(text1) == false )   _ACThrow("",NULL);
				a.Get(1,text);
				if( text.Compare(text2) == false )   _ACThrow("",NULL);
				a.Get(2,text);
				if( text.Compare(text3) == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(4,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				a.Delete1(3);
				a.Insert1(4,text);
				a.Get(4,text);
				if( text.Compare("test") == false )   _ACThrow("",NULL);
				a.Get(3,text);
				if( text.Compare("1234567890") == false )   _ACThrow("",NULL);
				text.SetCstring("CTextHandler.cp");
				a.Set(0,text);
				a.Set(1,text);
				AText	text_;
				text_.SetCstring("CTextHandler.cp ");
				a.Set(0,text_);
				if( a.Find(text) != 1 )   _ACThrow("",NULL);
			}
			
			a.DeleteAll();
			
			{
				AText	text1("a");
				AText	text2("ab");
				AText	text3("abc");
				AText	text4("abcd");
				AText	text5("abcde");
				AText	text6("abcdef");
				AText	text7("abcdefghijklmlopqraaaaaaaaaaaaaaaaaaaaaa");
				a.Add(text1);
				a.Add(text2);
				a.Add(text3);
				a.Add(text4);
				a.Add(text5);
				a.Add(text6);
				a.Add(text7);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != 4 )   _ACThrow("",NULL);
				if( a.Find(text6) != 5 )   _ACThrow("",NULL);
				if( a.Find(text7) != 6 )   _ACThrow("",NULL);
				a.Delete1(5);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != 4 )   _ACThrow("",NULL);
				if( a.Find(text6) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text7) != 5 )   _ACThrow("",NULL);
				a.Delete1(4);
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text6) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text7) != 4 )   _ACThrow("",NULL);
				a.MakeHash();
				if( a.Find(text1) != 0 )   _ACThrow("",NULL);
				if( a.Find(text2) != 1 )   _ACThrow("",NULL);
				if( a.Find(text3) != 2 )   _ACThrow("",NULL);
				if( a.Find(text4) != 3 )   _ACThrow("",NULL);
				if( a.Find(text5) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text6) != kIndex_Invalid )   _ACThrow("",NULL);
				if( a.Find(text7) != 4 )   _ACThrow("",NULL);
				AText	text;
				text.SetCstring("CTextHandler.cp");
				a.Set(0,text);
				a.Set(1,text);
				AText	text_;
				text_.SetCstring("CTextHandler.cp ");
				a.Set(0,text_);
				if( a.Find(text) != 1 )   _ACThrow("",NULL);
				a.Insert1(0,text_);
			}
		}
		//AText
		_AInfo("ClassTest: AText",NULL);
		{
			AText	a;
			a.SetCstring("/test1/test2.txt");
			ATextArray	textArray;
			a.Explode('/',textArray);
			AText	text;
			textArray.Get(0,text);
			if( text.Compare("") == false )   _ACThrow("",NULL);
			textArray.Get(1,text);
			if( text.Compare("test1") == false )   _ACThrow("",NULL);
			textArray.Get(2,text);
			if( text.Compare("test2.txt") == false )   _ACThrow("",NULL);
			
			a.SetCstring("test1/test2/");
			a.Explode('/',textArray);
			textArray.Get(0,text);
			if( text.Compare("test1") == false )   _ACThrow("",NULL);
			textArray.Get(1,text);
			if( text.Compare("test2") == false )   _ACThrow("",NULL);
			textArray.Get(2,text);
			if( text.Compare("") == false )   _ACThrow("",NULL);
			{
				AText	a;
				AText	text1, text2;
				a.AddCstring("test.html");
				a.GetSuffix(text1);
				text2.AddCstring(".html");
				if( text1.Compare(text2) == false )   _ACThrow("",NULL);
				a.SetCstring("test.asp.html");
				a.GetSuffix(text1);
				if( text1.Compare(text2) == false )   _ACThrow("",NULL);
			}
			{
				AText	a;
				a.SetCstring("ftp://user@ftp.xxx.yyy/path/path/file");
				AText	text;
				a.GetFTPURLServer(text);
				if( text.Compare("ftp.xxx.yyy") == false )   _ACThrow("",NULL);
				a.GetFTPURLUser(text);
				if( text.Compare("user") == false )   _ACThrow("",NULL);
				a.GetFTPURLPath(text);
				if( text.Compare("/path/path/file") == false )   _ACThrow("",NULL);
			}
			{
				AText	a;
				a.SetCstring("ftp://ftp.xxx.yyy/");
				AText	text;
				a.GetFTPURLServer(text);
				if( text.Compare("ftp.xxx.yyy") == false )   _ACThrow("",NULL);
				a.GetFTPURLUser(text);
				if( text.Compare("") == false )   _ACThrow("",NULL);
				a.GetFTPURLPath(text);
				if( text.Compare("/") == false )   _ACThrow("",NULL);
			}
			{
				AText	a;
				a.SetCstring("ftp://abcdefg@ftp.xxx.yyy");
				AText	text;
				a.GetFTPURLServer(text);
				if( text.Compare("ftp.xxx.yyy") == false )   _ACThrow("",NULL);
				a.GetFTPURLUser(text);
				if( text.Compare("abcdefg") == false )   _ACThrow("",NULL);
				a.GetFTPURLPath(text);
				if( text.Compare("") == false )   _ACThrow("",NULL);
			}
			//B0313
			{
				AText	a;
				a.SetCstring("aaa\rbbbbb\r\nccc1\nddd23\n\reee456");
				a.ConvertReturnCodeToCR();
				if( a.Compare("aaa\rbbbbb\rccc1\rddd23\r\reee456") == false )   _ACThrow("ConvertReturnCodeToCR() error",NULL);
				
				a.SetCstring("\r");
				a.ConvertReturnCodeToCR();
				if( a.Compare("\r") == false )   _ACThrow("ConvertReturnCodeToCR() error",NULL);
				
				a.SetCstring("\r\n");
				a.ConvertReturnCodeToCR();
				if( a.Compare("\r") == false )   _ACThrow("ConvertReturnCodeToCR() error",NULL);
				
				a.SetCstring("\n\n");
				a.ConvertReturnCodeToCR();
				if( a.Compare("\r\r") == false )   _ACThrow("ConvertReturnCodeToCR() error",NULL);
				
				a.SetCstring("\n\r");
				a.ConvertReturnCodeToCR();
				if( a.Compare("\r\r") == false )   _ACThrow("ConvertReturnCodeToCR() error",NULL);
				
				a.SetCstring("\n");
				a.ConvertReturnCodeToCR();
				if( a.Compare("\r") == false )   _ACThrow("ConvertReturnCodeToCR() error",NULL);
				
				a.SetCstring("\r\n\r\n\r\n\r\n\r\n");
				a.ConvertReturnCodeToCR();
				if( a.Compare("\r\r\r\r\r") == false )   _ACThrow("ConvertReturnCodeToCR() error",NULL);
				
			}
			//InsertFromUTF16TextPtr()
			{
				AUTF16Char	ch[16];
				ch[0] = 'a';
				ch[1] = 'b';
				AText	a;
				a.InsertFromUTF16TextPtr(0,ch,4);
				if( a.Compare("ab") == false )   _ACThrow("",NULL);
				a.InsertFromUTF16TextPtr(0,&(ch[1]),2);
				if( a.Compare("bab") == false )   _ACThrow("",NULL);
				a.InsertFromUTF16TextPtr(2,ch,4);
				if( a.Compare("baabb") == false )   _ACThrow("",NULL);
			}
			
			//UCS4<->UTF8 #427
			for( AUCS4Char ch = 0; ch < 0x200000; ch++ )
			{
				//UCS4����UTF8�֕ϊ�
				AText	text;
				text.AddFromUCS4Char(ch);
				//UTF8����UCS4�֕ϊ�
				AUCS4Char	c = 0;
				if( text.Convert1CharToUCS4(0,c) != text.GetItemCount() )   _ACThrow("",NULL);
				if( c != ch )   _ACThrow("",NULL);
			}
		}
		
		//TextEncoding Test
		_AInfo("TextEncoding Test",NULL);
		{
			/*AFileAcc	folder;
			folder.Specify("/TextEncodingTestMaster");
			
			AFileAcc	file;
			AText	utf8text;
			file.SpecifyChild(folder,"utf8");
			file.ReadTo(utf8text);
			utf8text.ConvertReturnCodeToCR();
			
			AText	text;
			file.SpecifyChild(folder,"sjis");
			file.ReadTo(text);
			ABool	fallback;
			text.ConvertToUTF8CR(ATextEncodingWrapper::GetSJISTextEncoding(),true,fallback);
			
			//file.SpecifyChild(folder,"xxx");
			//file.CreateFile();
			//file.WriteFile(text);
			
			if( text.Compare(utf8text) == false )   _ACThrow("text encoding error",NULL);*/
			
			AFileAcc	folder;
			//folder.Specify("/tectest");
			//folder.CreateFolder();
			folder.SpecifyAsUnitTestFolder();
			
			AFileAcc	file1;
			file1.SpecifyChild(folder,"file1");
			file1.CreateFile();
			AText	text1;
			MakeAllSJISCharText(text1);
			text1.Add(13);
			text1.Add(10);
			if( text1.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding()) == false )   _ACThrow("",NULL);
			AText	ch1;//�ŏ��̕������r�i�S�p�X�y�[�X�j
			ch1.Add(0xE3);
			ch1.Add(0x80);
			ch1.Add(0x80);
			if( text1.CompareMin(ch1) == false )   _ACThrow("",NULL);
			file1.WriteFile(text1);
			
			AFileAcc	file2;
			file2.SpecifyChild(folder,"file2");
			file2.CreateFile();
			AText	text2;
			MakeAllSJISCharText(text2);
			text2.Add(10);
			file2.WriteFile(text2);
			
			AText	utf8text;
			file1.ReadTo(utf8text);
			utf8text.ConvertReturnCodeToCR();
			
			AText	sjistext;
			file2.ReadTo(sjistext);
			ABool	fallback;
			sjistext.ConvertToUTF8CR(ATextEncodingWrapper::GetSJISTextEncoding(),true,fallback);
			
			if( sjistext.Compare(utf8text) == false )   _ACThrow("text encoding error",NULL);
			
		}
		{
			//UTF�ԕϊ��e�X�g
			AText	text;
			MakeAllUnicodeCharText(text);//UTF8
			AText	origtext;
			origtext.SetText(text);
			text.ConvertFromUTF8(ATextEncodingWrapper::GetUTF16TextEncoding());
			text.ConvertToUTF8(ATextEncodingWrapper::GetUTF16TextEncoding());
			text.ConvertFromUTF8(ATextEncodingWrapper::GetUTF16BETextEncoding());
			text.ConvertToUTF8(ATextEncodingWrapper::GetUTF16BETextEncoding());
			text.ConvertFromUTF8(ATextEncodingWrapper::GetUTF16LETextEncoding());
			text.ConvertToUTF8(ATextEncodingWrapper::GetUTF16LETextEncoding());
			if( text.Compare(origtext) == false )   _ACThrow("tec error",NULL);
		}
		{
			//JIS�n+UTF�ϊ��e�X�g
			AText	text;
			MakeAllSJISCharText(text);
			text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			AText	origtext;
			origtext.SetText(text);
			//
			text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			text.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			text.ConvertFromUTF8(ATextEncodingWrapper::GetJISTextEncoding());
			text.ConvertToUTF8(ATextEncodingWrapper::GetJISTextEncoding());
			text.ConvertFromUTF8(ATextEncodingWrapper::GetEUCTextEncoding());
			text.ConvertToUTF8(ATextEncodingWrapper::GetEUCTextEncoding());
			text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISX0213TextEncoding());
			text.ConvertToUTF8(ATextEncodingWrapper::GetSJISX0213TextEncoding());
			if( text.Compare(origtext) == false )   _ACThrow("tec error",NULL);
			//
			text.ConvertFromUTF8(ATextEncodingWrapper::GetUTF16TextEncoding());
			text.ConvertToUTF8(ATextEncodingWrapper::GetUTF16TextEncoding());
			text.ConvertFromUTF8(ATextEncodingWrapper::GetUTF16BETextEncoding());
			text.ConvertToUTF8(ATextEncodingWrapper::GetUTF16BETextEncoding());
			text.ConvertFromUTF8(ATextEncodingWrapper::GetUTF16LETextEncoding());
			text.ConvertToUTF8(ATextEncodingWrapper::GetUTF16LETextEncoding());
			if( text.Compare(origtext) == false )   _ACThrow("tec error",NULL);
			//#1040
			AText	correcttext("\\~\\~");
			origtext.SetCstring("\\~");
			origtext.Add(0xC2);
			origtext.Add(0xA5);
			origtext.Add(0xE2);
			origtext.Add(0x80);
			origtext.Add(0xBE);
			//SJIS
			text.SetText(origtext);
			if( text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding()) == false )   _ACThrow("tec error 1",NULL);
			if( text.Compare(correcttext) == false )   _ACThrow("tec error 2",NULL);
			//JIS
			text.SetText(origtext);
			if( text.ConvertFromUTF8(ATextEncodingWrapper::GetJISTextEncoding()) == false )   _ACThrow("tec error 3",NULL);
			if( text.Compare(correcttext) == false )   _ACThrow("tec error 4",NULL);
			//EUC
			text.SetText(origtext);
			if( text.ConvertFromUTF8(ATextEncodingWrapper::GetEUCTextEncoding()) == false )   _ACThrow("tec error 5",NULL);
			if( text.Compare(correcttext) == false )   _ACThrow("tec error 6",NULL);
			//SJIS 0213
			text.SetText(origtext);
			if( text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISX0213TextEncoding()) == false )   _ACThrow("tec error 7",NULL);
			if( text.Compare(correcttext) == false )   _ACThrow("tec error 8",NULL);
			//kCFStringEncodingMacJapanese
			text.SetText(origtext);
			if( text.ConvertFromUTF8(kCFStringEncodingMacJapanese) == false )   _ACThrow("tec error 9",NULL);
			if( text.Compare(correcttext) == false )   _ACThrow("tec error 10",NULL);
			//kCFStringEncodingShiftJIS
			text.SetText(origtext);
			if( text.ConvertFromUTF8(kCFStringEncodingShiftJIS) == false )   _ACThrow("tec error 11",NULL);
			if( text.Compare(correcttext) == false )   _ACThrow("tec error 12",NULL);
			//kCFStringEncodingShiftJIS_X0213
			text.SetText(origtext);
			if( text.ConvertFromUTF8(kCFStringEncodingShiftJIS_X0213) == false )   _ACThrow("tec error 13",NULL);
			if( text.Compare(correcttext) == false )   _ACThrow("tec error 14",NULL);
			/*
			//kCFStringEncodingShiftJIS_X0213_MenKuTen
			text.SetText(origtext);
			if( text.ConvertFromUTF8(kCFStringEncodingShiftJIS_X0213_MenKuTen) == false )   _ACThrow("tec error 15",NULL);
			if( text.Compare(correcttext) == false )   _ACThrow("tec error 16",NULL);
			*/
			//kCFStringEncodingDOSJapanese
			text.SetText(origtext);
			if( text.ConvertFromUTF8(kCFStringEncodingDOSJapanese) == false )   _ACThrow("tec error 17",NULL);
			if( text.Compare(correcttext) == false )   _ACThrow("tec error 18",NULL);
			//kCFStringEncodingEUC_JP
			text.SetText(origtext);
			if( text.ConvertFromUTF8(kCFStringEncodingEUC_JP) == false )   _ACThrow("tec error 19",NULL);
			if( text.Compare(correcttext) == false )   _ACThrow("tec error 20",NULL);
			//kCFStringEncodingISO_2022_JP
			text.SetText(origtext);
			if( text.ConvertFromUTF8(kCFStringEncodingISO_2022_JP) == false )   _ACThrow("tec error 21",NULL);
			if( text.Compare(correcttext) == false )   _ACThrow("tec error 22",NULL);
			
			if( AEnvWrapper::GetOSVersion() >= kOSVersion_MacOSX_10_6 )
			{
				//kCFStringEncodingISO_2022_JP_1
				text.SetText(origtext);
				if( text.ConvertFromUTF8(kCFStringEncodingISO_2022_JP_1) == false )   _ACThrow("tec error 23",NULL);
				if( text.Compare(correcttext) == false )   _ACThrow("tec error 24",NULL);
				//kCFStringEncodingISO_2022_JP_2
				text.SetText(origtext);
				if( text.ConvertFromUTF8(kCFStringEncodingISO_2022_JP_2) == false )   _ACThrow("tec error 25",NULL);
				if( text.Compare(correcttext) == false )   _ACThrow("tec error 26",NULL);
				/*
				//kCFStringEncodingISO_2022_JP_3
				text.SetText(origtext);
				if( text.ConvertFromUTF8(kCFStringEncodingISO_2022_JP_3) == false )   _ACThrow("tec error 28",NULL);
				if( text.Compare(correcttext) == false )   _ACThrow("tec error 29",NULL);
				*/
			}
		}
		//#200
		{
			AText	text;
			for( AUTF16Char	ch = 0xFF01; ch <= 0xFF5E; ch++ )
			{
				AText	t;
				t.SetTextFromUTF16CharPtr(&ch,2);
				text.AddText(t);
			}
			text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			if( text.Compare(	"�I�������������i�j���{�C�|�D�^"
								"�O�P�Q�R�S�T�U�V�W�X�F�G�������H"
								"���`�a�b�c�d�e�f�g�h�i�j�k�l�m�n"
								"�o�p�q�r�s�t�u�v�w�x�y�m�_�n�O�Q"
								"�M������������������������������"
								"�����������������������o�b�p�`") == false )   _ACThrow("tec error",NULL);
		}
		{
			AText	text;
			for( AUTF16Char	ch = 0xFF61; ch <= 0xFF9F; ch++ )
			{
				AText	t;
				t.SetTextFromUTF16CharPtr(&ch,2);
				text.AddText(t);
			}
			text.ConvertFromUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			if( text.Compare(	"���������������"
								"����������������"
								"����������������"
								"����������������") == false )   _ACThrow("tec error",NULL);
		}
		
		//RegExp Test
		_AInfo("RegExp Test",NULL);
		if( TestRegExp() == false )  _ACThrow("reg exp error",NULL);
		_AInfo("RegExp Test OK",NULL);
		
		//B0390
		{
			AText	text;
			/*
			//�e�X�g���p sjis
			text.Add(0xC3);
			text.Add(0xBD);
			text.Add(0xC4);
			text.Add(0x0D);
			text.Add(0x0D);
			ATextEncoding	tec = ATextEncodingWrapper::GetUTF8TextEncoding();
			text.GuessTextEncoding(true,true,true,false,tec);
			if( tec != ATextEncodingWrapper::GetSJISTextEncoding() )   _ACThrow("GuessTextEncoding error",NULL);
			*/
			//wmail sjis
			text.DeleteAll();
			text.Add(0x54);
			text.Add(0x6F);
			text.Add(0x3A);
			text.Add(0x20);
			text.Add(0x0A);
			text.Add(0x8F);
			text.Add(0x90);
			text.Add(0x96);
			text.Add(0xBC);
			text.Add(0x0A);
			ATextEncoding	tec = ATextEncodingWrapper::GetUTF8TextEncoding();
			text.GuessTextEncoding(tec);
			if( tec != ATextEncodingWrapper::GetSJISTextEncoding() )   _ACThrow("GuessTextEncoding error",NULL);
			/*#1052 euc�̗D��x�͉������̂ł����̓R�����g�A�E�g
			//�h���b�O euc
			text.DeleteAll();
			text.Add(0xA5);
			text.Add(0xC9);
			text.Add(0xA5);
			text.Add(0xE9);
			text.Add(0xA5);
			text.Add(0xC3);
			text.Add(0xA5);
			text.Add(0xB0);
			text.Add(0x0D);
			tec = ATextEncodingWrapper::GetUTF8TextEncoding();
			text.GuessTextEncoding(tec);
			if( tec != ATextEncodingWrapper::GetEUCTextEncoding() )   _ACThrow("GuessTextEncoding error",NULL);
			*/
		}
		
		//#208
		{
			AText	text;
			//������JIS X 208�e�L�X�g���G���[���肳��Ȃ����Ƃ̊m�F SJIS
			MakeAllSJISCharText(text);
			for( AIndex i = 0; i < text.GetItemCount(); i += 2 )
			{
				AText	ch;
				text.GetText(i,2,ch);
				ABool	kishuIzon = true, hankakuKana = true;
				ATextEncoding	tec = ATextEncodingWrapper::GetSJISTextEncoding();
				ATextEncodingWrapper::CheckJISX0208(tec,ch,kishuIzon,hankakuKana);
				if( kishuIzon == true || hankakuKana == true )   _ACThrow("CheckJISX0208() error",NULL);
			}
			
			//������JIS X 208�e�L�X�g���G���[���肳��Ȃ����Ƃ̊m�F JIS
			MakeAllSJISCharText(text);
			for( AIndex i = 0; i < text.GetItemCount(); i += 2 )
			{
				AText	ch;
				text.GetText(i,2,ch);
				ch.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				ch.ConvertFromUTF8(ATextEncodingWrapper::GetJISTextEncoding());
				ABool	kishuIzon = true, hankakuKana = true;
				ATextEncoding	tec = ATextEncodingWrapper::GetJISTextEncoding();
				ATextEncodingWrapper::CheckJISX0208(tec,ch,kishuIzon,hankakuKana);
				if( kishuIzon == true || hankakuKana == true )   _ACThrow("CheckJISX0208() error",NULL);
			}
			
			//������JIS X 208�e�L�X�g���G���[���肳��Ȃ����Ƃ̊m�F EUC
			MakeAllSJISCharText(text);
			for( AIndex i = 0; i < text.GetItemCount(); i += 2 )
			{
				AText	ch;
				text.GetText(i,2,ch);
				ch.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				ch.ConvertFromUTF8(ATextEncodingWrapper::GetEUCTextEncoding());
				ABool	kishuIzon = true, hankakuKana = true;
				ATextEncoding	tec = ATextEncodingWrapper::GetEUCTextEncoding();
				ATextEncodingWrapper::CheckJISX0208(tec,ch,kishuIzon,hankakuKana);
				if( kishuIzon == true || hankakuKana == true )   _ACThrow("CheckJISX0208() error",NULL);
			}
			
			
			//���p�J�i���G���[���肳��邱�Ƃ̊m�F SJIS
			text.SetCstring(	"���������������"
								"����������������"
								"����������������"
								"����������������");
			for( AIndex i = 0; i < text.GetItemCount(); i++ )
			{
				AText	ch;
				text.GetText(i,1,ch);
				ABool	kishuIzon = true, hankakuKana = false;
				ATextEncoding	tec = ATextEncodingWrapper::GetSJISTextEncoding();
				ATextEncodingWrapper::CheckJISX0208(tec,ch,kishuIzon,hankakuKana);
				if( kishuIzon == true || hankakuKana == false )   _ACThrow("CheckJISX0208() error",NULL);
			}
			
			//���p�J�i���G���[���肳��邱�Ƃ̊m�F JIS
			text.SetCstring(	"���������������"
								"����������������"
								"����������������"
								"����������������");
			for( AIndex i = 0; i < text.GetItemCount(); i++ )
			{
				AText	ch;
				text.GetText(i,1,ch);
				ch.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				ch.ConvertFromUTF8(ATextEncodingWrapper::GetJISTextEncoding());
				ABool	kishuIzon = true, hankakuKana = false;
				ATextEncoding	tec = ATextEncodingWrapper::GetJISTextEncoding();
				ATextEncodingWrapper::CheckJISX0208(tec,ch,kishuIzon,hankakuKana);
				if( kishuIzon == true || hankakuKana == false )   _ACThrow("CheckJISX0208() error",NULL);
			}
			
			//���p�J�i���G���[���肳��邱�Ƃ̊m�F EUC
			text.SetCstring(	"���������������"
								"����������������"
								"����������������"
								"����������������");
			for( AIndex i = 0; i < text.GetItemCount(); i++ )
			{
				AText	ch;
				text.GetText(i,1,ch);
				ch.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				ch.ConvertFromUTF8(ATextEncodingWrapper::GetEUCTextEncoding());
				ABool	kishuIzon = true, hankakuKana = false;
				ATextEncoding	tec = ATextEncodingWrapper::GetEUCTextEncoding();
				ATextEncodingWrapper::CheckJISX0208(tec,ch,kishuIzon,hankakuKana);
				if( kishuIzon == true || hankakuKana == false )   _ACThrow("CheckJISX0208() error",NULL);
			}
			
			//���p�J�i��OK���肳��邱�Ƃ̊m�F UTF8
			text.SetCstring(	"���������������"
								"����������������"
								"����������������"
								"����������������");
			for( AIndex i = 0; i < text.GetItemCount(); i++ )
			{
				AText	ch;
				text.GetText(i,1,ch);
				ch.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				ABool	kishuIzon = true, hankakuKana = true;
				ATextEncoding	tec = ATextEncodingWrapper::GetUTF8TextEncoding();
				ATextEncodingWrapper::CheckJISX0208(tec,ch,kishuIzon,hankakuKana);
				if( kishuIzon == true || hankakuKana == true )   _ACThrow("CheckJISX0208() error",NULL);
			}
			
			
			//�@��ˑ��������G���[���肳��邱�Ƃ̊m�F SJIS
			text.SetCstring(	"�@�A�B�C�D�E�F�G�H�I�J�K�L�M�N�O�P�Q�R�S"
								"�T�U�V�W�X�Y�Z�[�\\�]"
								"�_�`�a�b�c�d�e�f�g�h�i�j�k�l�m�n�o�p�q�r�s�t�u"
								"�~��������������������������������");
			for( AIndex i = 0; i < text.GetItemCount(); i += 2 )
			{
				AText	ch;
				text.GetText(i,2,ch);
				ABool	kishuIzon = false, hankakuKana = true;
				ATextEncoding	tec = ATextEncodingWrapper::GetSJISTextEncoding();
				ATextEncodingWrapper::CheckJISX0208(tec,ch,kishuIzon,hankakuKana);
				if( kishuIzon == false || hankakuKana == true )   _ACThrow("CheckJISX0208() error",NULL);
			}
			/*#1052 JIS/EUC�ŋ@��ˑ������͖{�����݂��Ȃ��̂ł����̓R�����g�A�E�g
			//�@��ˑ��������G���[���肳��邱�Ƃ̊m�F JIS
			text.SetCstring(	"�@�A�B�C�D�E�F�G�H�I�J�K�L�M�N�O�P�Q�R�S"
								"�T�U�V�W�X�Y�Z�[�\\�]"
								"�_�`�a�b�c�d�e�f�g�h�i�j�k�l�m�n�o�p�q�r�s�t�u"
								"�~��������������������������������");
			for( AIndex i = 0; i < text.GetItemCount(); i += 2 )
			{
				AText	ch;
				text.GetText(i,2,ch);
				ch.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				ch.ConvertFromUTF8(ATextEncodingWrapper::GetJISTextEncoding());
				ABool	kishuIzon = false, hankakuKana = true;
				ATextEncoding	tec = ATextEncodingWrapper::GetJISTextEncoding();
				ATextEncodingWrapper::CheckJISX0208(tec,ch,kishuIzon,hankakuKana);
				if( kishuIzon == false || hankakuKana == true )   _ACThrow("CheckJISX0208() error",NULL);
			}
			
			//�@��ˑ��������G���[���肳��邱�Ƃ̊m�F EUC
			text.SetCstring(	"�@�A�B�C�D�E�F�G�H�I�J�K�L�M�N�O�P�Q�R�S"
								"�T�U�V�W�X�Y�Z�[�\\�]"
								"�_�`�a�b�c�d�e�f�g�h�i�j�k�l�m�n�o�p�q�r�s�t�u"
								"�~��������������������������������");
			for( AIndex i = 0; i < text.GetItemCount(); i += 2 )
			{
				AText	ch;
				text.GetText(i,2,ch);
				ch.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				ch.ConvertFromUTF8(ATextEncodingWrapper::GetEUCTextEncoding());
				ABool	kishuIzon = false, hankakuKana = true;
				ATextEncoding	tec = ATextEncodingWrapper::GetEUCTextEncoding();
				ATextEncodingWrapper::CheckJISX0208(tec,ch,kishuIzon,hankakuKana);
				if( kishuIzon == false || hankakuKana == true )   _ACThrow("CheckJISX0208() error",NULL);
			}
			*/
			//�@��ˑ��������G���[���肳��邱�Ƃ̊m�F UTF8
			text.SetCstring(	"�@�A�B�C�D�E�F�G�H�I�J�K�L�M�N�O�P�Q�R�S"
								"�T�U�V�W�X�Y�Z�[�\\�]"
								"�_�`�a�b�c�d�e�f�g�h�i�j�k�l�m�n�o�p�q�r�s�t�u"
								"�~��������������������������������");
			for( AIndex i = 0; i < text.GetItemCount(); i += 2 )
			{
				AText	ch;
				text.GetText(i,2,ch);
				ch.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
				ABool	kishuIzon = true, hankakuKana = true;
				ATextEncoding	tec = ATextEncodingWrapper::GetUTF8TextEncoding();
				ATextEncodingWrapper::CheckJISX0208(tec,ch,kishuIzon,hankakuKana);
				if( kishuIzon == true || hankakuKana == true )   _ACThrow("CheckJISX0208() error",NULL);
			}
			
		}
		
		//#325
		{
			//�e�L�X�g�G�f�B�b�g�ō쐬�����A�e�X�g�p���p�J�i�R�[�h������e�L�X�g��ǂݍ���
			AFileAcc	testfolder;
			AFileWrapper::GetLocalizedResourceFile("TestPatterns",testfolder);
			AFileAcc	file;
			file.SpecifyChild(testfolder,"HankakuKanaWithTextEditJIS.txt");
			AText	origtext;
			file.ReadTo(origtext);
			origtext.ConvertLineEndToCR();
			//
			AText	text;
			text.SetText(origtext);
			//UTF-8�֕ϊ�
			text.ConvertToUTF8(ATextEncodingWrapper::GetJISTextEncoding());
			//���L�̃e�L�X�g�Ɣ�r�i���̃t�@�C����SJIS�Ȃ̂�UTF8�֕ϊ����Ă����r�j
			AText	refText;
			refText.SetCstring(
					"���������������������������������������������������������������\r"
					"���������������������������������������������������������������ascii\r"
					"��������������������������������������������������������������ߊ���\r"
					"ascii���������������������������������������������������������������\r"
					"�������������������������������������������������������������������\r"
					"����ascii���������������������������������������������������������������ascii\r"
					"ascii������������������������������������������������������������������ߊ���\r"
					"�������ascii��������������������ascii���������ascii�������������ascii������������ascii����ascii����������������������\r");
			refText.ConvertToUTF8(ATextEncodingWrapper::GetSJISTextEncoding());
			if( text.Compare(refText) == false )   _ACThrow("#325 error",NULL);
			//JIS�ɖ߂�
			text.ConvertFromUTF8(ATextEncodingWrapper::GetJISTextEncoding());
			//���X�̃e�L�X�g�Ɣ�r
			if( text.Compare(origtext) == false )   _ACThrow("#325 error",NULL);
		}
		
		//#688
		{
			ALocalRect	rect1 = {80,100,250,200}, rect2 = {140,150,310,300};
			ALocalRect	intersect = {0,0,0,0};
			if( GetIntersectRect(rect1,rect2,intersect) == false )   _ACThrow("GetIntersectRect() error",NULL);
			if( intersect.left != 140 || intersect.right != 250 || intersect.top != 150 || intersect.bottom != 200 )   _ACThrow("GetIntersectRect() error",NULL);
		}
		{
			ALocalRect	rect2 = {80,100,250,200}, rect1 = {140,150,310,300};
			ALocalRect	intersect = {0,0,0,0};
			if( GetIntersectRect(rect1,rect2,intersect) == false )   _ACThrow("GetIntersectRect() error",NULL);
			if( intersect.left != 140 || intersect.right != 250 || intersect.top != 150 || intersect.bottom != 200 )   _ACThrow("GetIntersectRect() error",NULL);
		}
		
		//#693
		{
			ATextArray	textarray;
			AText	a("a"), b("b"), c("c");
			textarray.Add(a);
			textarray.Add(b);
			textarray.Add(c);
			textarray.MoveObject(1,0);
			AText	t;
			textarray.Implode(',',t);
			if( t.Compare("b,a,c,") == false )   _ACThrow("#693",NULL);
			ATextArray	textarray2;
			textarray2.SetFromTextArray(textarray);
			textarray.AddFromTextArray(textarray2);
			textarray.Implode(',',t);
			if( t.Compare("b,a,c,b,a,c,") == false )   _ACThrow("#693",NULL);
			textarray.ExplodeFromText(t,',');
			textarray.Delete1(textarray.GetItemCount()-1);//�Ō�������i�Ō��,������̂ŋ󔒃e�L�X�g���t���Ă���̂Łj
			textarray.Implode(',',t);
			if( t.Compare("b,a,c,b,a,c,") == false )   _ACThrow("#693",NULL);
			textarray.Sort(true);
			textarray.Implode(',',t);
			if( t.Compare("a,a,b,b,c,c,") == false )   _ACThrow("#693",NULL);
			textarray.Sort(false);
			textarray.Implode(',',t);
			if( t.Compare("c,c,b,b,a,a,") == false )   _ACThrow("#693",NULL);
			textarray.SwapObject(0,5);
			textarray.Implode(',',t);
			if( t.Compare("a,c,b,b,a,c,") == false )   _ACThrow("#693",NULL);
			
			//ATextArray::Reserve()
			textarray.Reserve(2,3);
			textarray.Set(2,c);
			textarray.Reserve(0,1);
			textarray.Reserve(textarray.GetItemCount(),1);
			textarray.Implode(',',t);
			if( t.Compare(",a,c,c,,,b,b,a,c,,") == false )   _ACThrow("ATextArray::Reserve()",NULL);
		}
		
		//GetCountOfChar()
		{
			AText	t("123\r456\r789\r");
			if( t.GetCountOfChar('\r',0,t.GetItemCount()) != 3 )   _ACThrow("GetCountOfChar()",NULL);
			if( t.GetCountOfChar('\r',3,t.GetItemCount()-1-3) != 2 )   _ACThrow("GetCountOfChar()",NULL);
		}
	}
	catch(...)
	{
		result = false;
	}
	return result;
}






