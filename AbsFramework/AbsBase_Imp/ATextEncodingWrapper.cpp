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

ATextEncodingWrapper

MacOSX�p�e�L�X�g�G���R�[�f�B���O�������b�p�[


*/

#include "stdafx.h"

#include "ATextEncodingWrapper.h"

extern ABase*	gBase;

#if IMPLEMENTATION_FOR_MACOSX
ATextEncoding	ATextEncodingWrapper::sUTF16TextEncoding = NULL;
//#1040 ATextEncoding	ATextEncodingWrapper::sUTF16CanonicalCompTextEncoding = NULL;//#1044
ATextEncoding	ATextEncodingWrapper::sUTF8TextEncoding = NULL;
ATextEncoding	ATextEncodingWrapper::sJISTextEncoding = NULL;
ATextEncoding	ATextEncodingWrapper::sEUCTextEncoding = NULL;
ATextEncoding	ATextEncodingWrapper::sSJISTextEncoding = NULL;
ATextEncoding	ATextEncodingWrapper::sSJISX0213TextEncoding = NULL;
ATextEncoding	ATextEncodingWrapper::sUTF16LETextEncoding = NULL;
ATextEncoding	ATextEncodingWrapper::sUTF16BETextEncoding = NULL;//B0377
TECObjectRef	ATextEncodingWrapper::sConverterFromUTF8ToUTF16;
TECObjectRef	ATextEncodingWrapper::sConverterFromUTF16ToUTF8;
#endif

#pragma mark ===========================
#pragma mark [�N���X]ATextEncodingWrapper
#pragma mark ===========================

#pragma mark ---Init

/**
������
*/
void	ATextEncodingWrapper::Init()
{
#if IMPLEMENTATION_FOR_MACOSX
	GetTextEncodingFromName("ISO-2022-JP",sJISTextEncoding);
	GetTextEncodingFromName("EUC-JP",sEUCTextEncoding);
	GetTextEncodingFromName("Shift_JIS",sSJISTextEncoding);
	GetTextEncodingFromName("Shift_JISX0213",sSJISX0213TextEncoding);
	GetTextEncodingFromName("UTF-8",sUTF8TextEncoding);
	GetTextEncodingFromName("UTF-16",sUTF16TextEncoding);
	GetTextEncodingFromName("UTF-16LE",sUTF16LETextEncoding);
	GetTextEncodingFromName("UTF-16BE",sUTF16BETextEncoding);//B0377
	/*#1040
	//#1044
	sUTF16CanonicalCompTextEncoding = ::CreateTextEncoding(kTextEncodingUnicodeDefault,kUnicodeCanonicalCompVariant,kUnicode16BitFormat);
	*/
	::TECCreateConverter(&sConverterFromUTF8ToUTF16,sUTF8TextEncoding,sUTF16TextEncoding);
	::TECCreateConverter(&sConverterFromUTF16ToUTF8,sUTF16TextEncoding,sUTF8TextEncoding);
	
#endif
#if IMPLEMENTATION_FOR_WINDOWS
	//�Q�l�Fhttp://msdn.microsoft.com/en-us/library/ms776446(VS.85).aspx 
	RegisterTextEncodingName("UTF-8",65001);
	RegisterTextEncodingName("UTF-16",1200);
	RegisterTextEncodingName("UTF-16LE",1200);
	RegisterTextEncodingName("UTF-16BE",1201);
	RegisterTextEncodingName("Shift_JIS",932);
	RegisterTextEncodingName("ISO-2022-JP",50222);
	RegisterTextEncodingName("EUC-JP",51932);
	RegisterTextEncodingName("ISO-8859-1",28591);
	RegisterTextEncodingName("ISO-8859-2",28592);
	RegisterTextEncodingName("ISO-8859-4",28594);
	RegisterTextEncodingName("ISO-8859-13",28603);
	RegisterTextEncodingName("ISO-8859-15",28605);
	RegisterTextEncodingName("windows-1250",1250);//win
	RegisterTextEncodingName("windows-1252",1252);//win
	RegisterTextEncodingName("macintosh",10000);
	RegisterTextEncodingName("ISO-8859-5",28595);
	RegisterTextEncodingName("KOI8-R",20866);
	RegisterTextEncodingName("x-mac-cyrillic",10007);
	RegisterTextEncodingName("windows-1251",1251);
	RegisterTextEncodingName("ISO-8859-7",28597);
	RegisterTextEncodingName("x-mac-greek",10006);
	RegisterTextEncodingName("windows-1253",1253);
	RegisterTextEncodingName("ISO-8859-9",28599);
	RegisterTextEncodingName("x-mac-turkish",10081);
	RegisterTextEncodingName("windows-1254",1254);
	RegisterTextEncodingName("Big5",950);
	RegisterTextEncodingName("HZ-GB-2312",52936);
	RegisterTextEncodingName("GB18030",54936);
	//
	RegisterTextEncodingName("IBM037",037);
	RegisterTextEncodingName("IBM437",437);
	RegisterTextEncodingName("IBM500",500);
	RegisterTextEncodingName("ASMO-708",708);
	RegisterTextEncodingName("DOS-720",720);
	RegisterTextEncodingName("ibm737",737);
	RegisterTextEncodingName("ibm775",775);
	RegisterTextEncodingName("ibm850",850);
	RegisterTextEncodingName("ibm852",852);
	RegisterTextEncodingName("IBM855",855);
	RegisterTextEncodingName("ibm857",857);
	RegisterTextEncodingName("IBM00858",858);
	RegisterTextEncodingName("IBM860",860);
	RegisterTextEncodingName("ibm861",861);
	RegisterTextEncodingName("DOS-862",862);
	RegisterTextEncodingName("IBM863",863);
	RegisterTextEncodingName("IBM864",864);
	RegisterTextEncodingName("IBM865",865);
	RegisterTextEncodingName("cp866",866);
	RegisterTextEncodingName("ibm869",869);
	RegisterTextEncodingName("IBM870",870);
	RegisterTextEncodingName("windows-874",874);
	RegisterTextEncodingName("cp875",875);
	RegisterTextEncodingName("shift_jis",932);
	RegisterTextEncodingName("gb2312",936);
	RegisterTextEncodingName("ks_c_5601-1987",949);
	RegisterTextEncodingName("big5",950);
	RegisterTextEncodingName("IBM1026",1026);
	RegisterTextEncodingName("IBM01047",1047);
	RegisterTextEncodingName("IBM01140",1140);
	RegisterTextEncodingName("IBM01141",1141);
	RegisterTextEncodingName("IBM01142",1142);
	RegisterTextEncodingName("IBM01143",1143);
	RegisterTextEncodingName("IBM01144",1144);
	RegisterTextEncodingName("IBM01145",1145);
	RegisterTextEncodingName("IBM01146",1146);
	RegisterTextEncodingName("IBM01147",1147);
	RegisterTextEncodingName("IBM01148",1148);
	RegisterTextEncodingName("IBM01149",1149);
	RegisterTextEncodingName("utf-16",1200);
	RegisterTextEncodingName("unicodeFFFE",1201);
	RegisterTextEncodingName("windows-1250",1250);
	RegisterTextEncodingName("windows-1251",1251);
	RegisterTextEncodingName("windows-1252",1252);
	RegisterTextEncodingName("windows-1253",1253);
	RegisterTextEncodingName("windows-1254",1254);
	RegisterTextEncodingName("windows-1255",1255);
	RegisterTextEncodingName("windows-1256",1256);
	RegisterTextEncodingName("windows-1257",1257);
	RegisterTextEncodingName("windows-1258",1258);
	RegisterTextEncodingName("Johab",1361);
	RegisterTextEncodingName("macintosh",10000);
	RegisterTextEncodingName("x-mac-japanese",10001);
	RegisterTextEncodingName("x-mac-chinesetrad",10002);
	RegisterTextEncodingName("x-mac-korean",10003);
	RegisterTextEncodingName("x-mac-arabic",10004);
	RegisterTextEncodingName("x-mac-hebrew",10005);
	RegisterTextEncodingName("x-mac-greek",10006);
	RegisterTextEncodingName("x-mac-cyrillic",10007);
	RegisterTextEncodingName("x-mac-chinesesimp",10008);
	RegisterTextEncodingName("x-mac-romanian",10010);
	RegisterTextEncodingName("x-mac-ukrainian",10017);
	RegisterTextEncodingName("x-mac-thai",10021);
	RegisterTextEncodingName("x-mac-ce",10029);
	RegisterTextEncodingName("x-mac-icelandic",10079);
	RegisterTextEncodingName("x-mac-turkish",10081);
	RegisterTextEncodingName("x-mac-croatian",10082);
	RegisterTextEncodingName("utf-32",12000);
	RegisterTextEncodingName("utf-32BE",12001);
	RegisterTextEncodingName("x-Chinese_CNS",20000);
	RegisterTextEncodingName("x-cp20001",20001);
	RegisterTextEncodingName("x_Chinese-Eten",20002);
	RegisterTextEncodingName("x-cp20003",20003);
	RegisterTextEncodingName("x-cp20004",20004);
	RegisterTextEncodingName("x-cp20005",20005);
	RegisterTextEncodingName("x-IA5",20105);
	RegisterTextEncodingName("x-IA5-German",20106);
	RegisterTextEncodingName("x-IA5-Swedish",20107);
	RegisterTextEncodingName("x-IA5-Norwegian",20108);
	RegisterTextEncodingName("us-ascii",20127);
	RegisterTextEncodingName("x-cp20261",20261);
	RegisterTextEncodingName("x-cp20269",20269);
	RegisterTextEncodingName("IBM273",20273);
	RegisterTextEncodingName("IBM277",20277);
	RegisterTextEncodingName("IBM278",20278);
	RegisterTextEncodingName("IBM280",20280);
	RegisterTextEncodingName("IBM284",20284);
	RegisterTextEncodingName("IBM285",20285);
	RegisterTextEncodingName("IBM290",20290);
	RegisterTextEncodingName("IBM297",20297);
	RegisterTextEncodingName("IBM420",20420);
	RegisterTextEncodingName("IBM423",20423);
	RegisterTextEncodingName("IBM424",20424);
	RegisterTextEncodingName("x-EBCDIC-KoreanExtended",20833);
	RegisterTextEncodingName("IBM-Thai",20838);
	RegisterTextEncodingName("koi8-r",20866);
	RegisterTextEncodingName("IBM871",20871);
	RegisterTextEncodingName("IBM880",20880);
	RegisterTextEncodingName("IBM905",20905);
	RegisterTextEncodingName("IBM00924",20924);
	//50932�����L���ɂ���RegisterTextEncodingName("EUC-JP",20932);
	RegisterTextEncodingName("x-cp20936",20936);
	RegisterTextEncodingName("x-cp20949",20949);
	RegisterTextEncodingName("cp1025",21025);
	RegisterTextEncodingName("koi8-u",21866);
	RegisterTextEncodingName("iso-8859-1",28591);
	RegisterTextEncodingName("iso-8859-2",28592);
	RegisterTextEncodingName("iso-8859-3",28593);
	RegisterTextEncodingName("iso-8859-4",28594);
	RegisterTextEncodingName("iso-8859-5",28595);
	RegisterTextEncodingName("iso-8859-6",28596);
	RegisterTextEncodingName("iso-8859-7",28597);
	RegisterTextEncodingName("iso-8859-8",28598);
	RegisterTextEncodingName("iso-8859-9",28599);
	RegisterTextEncodingName("iso-8859-13",28603);
	RegisterTextEncodingName("iso-8859-15",28605);
	RegisterTextEncodingName("x-Europa",29001);
	RegisterTextEncodingName("iso-8859-8-i",38598);
	//RegisterTextEncodingName("iso-2022-jp",50220);
	RegisterTextEncodingName("csISO2022JP",50221);
	RegisterTextEncodingName("iso-2022-jp",50222);
	RegisterTextEncodingName("iso-2022-kr",50225);
	RegisterTextEncodingName("x-cp50227",50227);
	RegisterTextEncodingName("euc-jp",51932);
	RegisterTextEncodingName("EUC-CN",51936);
	RegisterTextEncodingName("euc-kr",51949);
	RegisterTextEncodingName("hz-gb-2312",52936);
	RegisterTextEncodingName("GB18030",54936);
	RegisterTextEncodingName("x-iscii-de",57002);
	RegisterTextEncodingName("x-iscii-be",57003);
	RegisterTextEncodingName("x-iscii-ta",57004);
	RegisterTextEncodingName("x-iscii-te",57005);
	RegisterTextEncodingName("x-iscii-as",57006);
	RegisterTextEncodingName("x-iscii-or",57007);
	RegisterTextEncodingName("x-iscii-ka",57008);
	RegisterTextEncodingName("x-iscii-ma",57009);
	RegisterTextEncodingName("x-iscii-gu",57010);
	RegisterTextEncodingName("x-iscii-pa",57011);
	RegisterTextEncodingName("utf-7",65000);
	RegisterTextEncodingName("utf-8",65001);
#endif
}

#if IMPLEMENTATION_FOR_WINDOWS
AHashTextArray	ATextEncodingWrapper::sTextEncodingArray_Name;
AArray<ATextEncoding>	ATextEncodingWrapper::sTextEncodingArray_CodePage;

/**
TextEncoding���o�^�iWindows�p�j
*/
void	ATextEncodingWrapper::RegisterTextEncodingName( AConstCharPtr inName, const ATextEncoding inCodePage )
{
	AText	name(inName);
	sTextEncodingArray_Name.Add(name);
	sTextEncodingArray_CodePage.Add(inCodePage);
}
#endif

/*ATextEncodingWrapper::~ATextEncodingWrapper()
{
	
	::TECDisposeConverter(mConverterFromUTF8ToUTF16);
	::TECDisposeConverter(mConverterFromUTF16ToUTF8);
}*/

#pragma mark ===========================

#pragma mark ---TextEncoding�ϊ�

/**
�w��TextEncoding->UTF8

ioText�̃e�L�X�g��inTextEncoding����UTF-8�ɕϊ����AioText�Ɋi�[����
@note �ϊ����s�̏ꍇ��false��Ԃ��B���̏ꍇ�AioText�̓��e�͔j�󂳂�Ă���B
*/
ABool	ATextEncodingWrapper::ConvertToUTF8( AText& ioText, const ATextEncoding inSrcTextEncoding, 
		const ABool inUseFallback, ABool& outFallbackUsed, const ABool inFromExternalData )//B0400
{
	if( ioText.GetItemCount() == 0 )   return true;
	
	ABool	result = true;
	outFallbackUsed = false;
	
	//���L�̃V�[�P���X�ɂ��inTextEncoding����UTF-8�֕ϊ�����
	//�@BOM����
	//�A�ꕔ�G���R�[�f�B���O�̏ꍇ�ASJIS�֕ϊ�
	//�BUTF16�֕ϊ�
	//�CUTF16��UTF8
	
	//�����r������ł̕����R�[�h���i�[
	ATextEncoding	curEncoding = inSrcTextEncoding;
	
	CFStringRef	strref = NULL;
	CFMutableStringRef	mstrref = NULL;
	try
	{
		//�@BOM����
		//UTF-8��BOM���܂�ł�����ABOM�폜����
		//B0000 2.1.7���ォ��BOMOffset�͖��g�p AIndex	BOMoffset = 0;
		if( curEncoding == GetUTF8TextEncoding() )
		{
			if( ioText.GetItemCount() >= 3 )
			{
				if( ioText.Get(0) == 0xEF && ioText.Get(1) == 0xBB && ioText.Get(2) == 0xBF )
				{
					ioText.Delete(0,3);
				}
			}
		}
		/*#1040 BOM�̈�����CFStringCreateWithBytes()�ɔC����i�������AUTF-8�̏ꍇ��CFStringCreateWithBytes()�����s���Ȃ��̂ŁA���BOM�폜���Ă���j
		//UTF-16��BOM���܂�ł�����ABOM�ɏ]���āA�g���Ă���}�V����Endian�ɍ��킹��UTF-16�ɕϊ�����
		if( curEncoding == GetUTF16TextEncoding() || curEncoding == GetUTF16LETextEncoding() || curEncoding == GetUTF16BETextEncoding() )
		{
			if( ioText.GetItemCount() >= 2 )
			{
				//BOM��FE FF
				if( ioText.Get(0) == 0xFE && ioText.Get(1) == 0xFF )
				{
					ioText.Delete(0,2);
					//IntelMac�Ȃ猳�f�[�^FE FF��FF FE�iFEFF��LE�z�u�j�ƂȂ�悤�ɂ���
					if( UTF16IsLE() )
					{
						ChangeByteOrder(ioText);
					}
				}
				//BOM��FF FE
				else if( ioText.Get(0) == 0xFF && ioText.Get(1) == 0xFE )
				{
					ioText.Delete(0,2);
					//PowerPCMac�Ȃ猳�f�[�^FF FE��FE FF�iFEFF��BE�z�u�j�ƂȂ�悤�ɂ���
					if( UTF16IsLE() == false )
					{
						ChangeByteOrder(ioText);
					}
				}
				//BOM���Ȃ��āAUTF16LE�w��Ȃ�A���f�[�^��LE�z�u�Ȃ̂ŁAPowerPCMac�Ɍ���Abyte swap
				else if( curEncoding == GetUTF16LETextEncoding() )
				{
					if( UTF16IsLE() == false )
					{
						ChangeByteOrder(ioText);
					}
				}
				//BOM���Ȃ��āAUTF16BE�w��Ȃ�A���f�[�^��BE�z�u�Ȃ̂ŁAIntelMac�Ɍ���Abyte swap B0377
				else if( curEncoding == GetUTF16BETextEncoding() )
				{
					if( UTF16IsLE() == true )
					{
						ChangeByteOrder(ioText);
					}
				}
				//BOM���Ȃ��āAUTF16�w��Ȃ�A���f�[�^��BE�z�u�Ȃ̂ŁAIntelMac�Ɍ���Abyte swap B0400
				//�������A�O���i�t�@�C���j����̃f�[�^�̏ꍇ�Ɍ���i'utxt'��TEC�ŕϊ������f�t�H���gUnicode16bit�f�[�^�́A�@��ˑ�BE/LE�ƂȂ邽�߁B�j
				else if( curEncoding == GetUTF16TextEncoding() && inFromExternalData == true )
				{
					if( UTF16IsLE() == true )
					{
						ChangeByteOrder(ioText);
					}
				}
				//�ȏ�ɂ��A�g���Ă���}�V����Endian�ɍ��킹��UTF-16�ɕϊ��I��
				curEncoding = GetUTF16TextEncoding();
			}
		}
		*/
		
		if( curEncoding != GetUTF8TextEncoding() )
		{
			//�ϊ����e�L�X�g�G���R�[�f�B���O�̃o�C�g�񂩂�CFString�𐶐�
			{
				AStTextPtr	intextptr(ioText,0,ioText.GetItemCount());
				strref = ::CFStringCreateWithBytes(kCFAllocatorDefault,intextptr.GetPtr(),intextptr.GetByteCount(),
												   curEncoding,inFromExternalData);
			}
			if( strref == NULL )
			{
				//CFStringCreateWithBytes()�ŕϊ��G���[�̂Ƃ��́A��API���g�p���āA�t�H�[���o�b�N�ϊ������݂�
				if( inUseFallback == true )
				{
					//JIS�̏ꍇ�AJIS��SJIS�ɕϊ�
					if( curEncoding == GetJISTextEncoding() )
					{
						//B0126
						ConvertFromJISToSJIS(ioText);
						curEncoding = kCFStringEncodingDOSJapanese;
					}
					//EUC�̏ꍇ�AEUC��SJIS�ɕϊ� B0141
					if( curEncoding == GetEUCTextEncoding() )
					{
						ConvertFromEUCToSJIS(ioText);
						curEncoding = kCFStringEncodingDOSJapanese;
					}
					//TextEncoding�����擾
					TextEncoding	tec = 0;
					AText	tecname;
					switch(curEncoding)
					{
					  case kCFStringEncodingMacJapanese:
						{
							tecname.SetCstring("x-mac-japanese");
							break;
						}
					  case kCFStringEncodingDOSJapanese:
					  case kCFStringEncodingShiftJIS:
						{
							tecname.SetCstring("Shift_JIS");
							break;
						}
					  case kCFStringEncodingShiftJIS_X0213:
						{
							tecname.SetCstring("Shift_JISX0213");
							break;
						}
					  default:
						{
							throw 0;
						}
					}
					//��API��TextEncoding�l���擾
					Str255	tecnamestr;
					tecname.GetPstring(tecnamestr,255);
					if( ::TECGetTextEncodingFromInternetName(&tec,tecnamestr) != noErr )   throw 0;
					//UTF16�֕ϊ�
					// �e�핶���R�[�h��UTF16
					{
						AText	buffer;
						if( TECConvertToUTF16(ioText,buffer,tec,inUseFallback,outFallbackUsed) == false )   throw 0;
						ioText.SwapContent(buffer);
					}
					
					//UTF16��UTF8
					AText	buffer;
					if( TECConvertUTFx(ioText,buffer,GetUTF16TextEncoding(),GetUTF8TextEncoding()) == false )   throw 0;
					ioText.SwapContent(buffer);
					return true;
				}
				//
				throw 0;
			}
			
			//CFMutableString����
			mstrref = ::CFStringCreateMutableCopy(kCFAllocatorDefault,0,strref);
			if( mstrref == NULL )   throw 0;
			
			//JIS�n���ʂ̕ϊ�
			if( IsSJISSeriesTextEncoding(inSrcTextEncoding) == true || 
				inSrcTextEncoding == GetJISTextEncoding() || 
				inSrcTextEncoding == GetEUCTextEncoding() )
			{
				ConvertCFMutableStringChar(mstrref,0xFFE1,0x00A3);//���p���փ}�b�s���O���ꂷ��
				ConvertCFMutableStringChar(mstrref,0xFFE0,0x00A2);//���p���փ}�b�s���O���ꂷ��
				ConvertCFMutableStringChar(mstrref,0xFF0D,0x2212);
				ConvertCFMutableStringChar(mstrref,0x2225,0x2016);
				ConvertCFMutableStringChar(mstrref,0xFF5E,0x301C);
				ConvertCFMutableStringChar(mstrref,0xFFE2,0x00AC);
				ConvertCFMutableStringChar(mstrref,0x2015,0x2014);//SJIS:0x815C��U+2014�i�S�p�_�b�V���j�փ}�b�s���O���ꂷ��
				ConvertCFMutableStringChar(mstrref,0x203E,0x007E);//SJIS:0x7E��U+007E�փ}�b�s���O���ꂷ��
				ConvertCFMutableStringChar(mstrref,0x00A5,0x005C);//#1106
			}
			
			//CFString�S�̂�range���擾
			CFRange	range = ::CFRangeMake(0,::CFStringGetLength(mstrref));
			
			//���݂�ioText��S�폜
			ioText.DeleteAll();
			
			//�ϊ���ɕK�v�ȃo�C�g�����擾
			// ioText.Reserve(0,::CFStringGetMaximumSizeForEncoding(::CFStringGetLength(mstrref),GetUTF8TextEncoding())+1024);
			CFIndex	resultLen = 0;
			::CFStringGetBytes(mstrref,range,GetUTF8TextEncoding(),0,false,NULL,0,&resultLen);
			
			//�ϊ��㕶����i�[��m��
			ioText.Reserve(0,resultLen);
			
			//UTF-8������擾
			{
				AStTextPtr	textptr(ioText,0,ioText.GetItemCount());
				CFIndex	index = ::CFStringGetBytes(mstrref,range,GetUTF8TextEncoding(),0,false,
												   textptr.GetPtr(), textptr.GetByteCount(), &resultLen );
				//�ϊ�������������CFString�̕������ƈ�v���Ă��Ȃ���΃G���[�Ƃ���
				if( index != ::CFStringGetLength(mstrref) )   throw 0;
			}
			
			//ioText���g�p�������폜�i���߂Ɋm�ۂ�����@�͎~�߂��̂ŕs�v�Ǝv�����A�O�̂��ߎc���j
			ioText.Delete(resultLen,ioText.GetItemCount()-resultLen);
			
			/*#1040
			//B0126 TECSetBasicOptions��MacOSX10.2�Ŏg�p�ł��Ȃ��̂ŁATECConvertText�͎g�p�s��
			//����āAConvertFromTextToUnicode��UTF16�ɕϊ��A���̌�AUTF8�ɕϊ�����
			
			//�A�ꕔ�G���R�[�f�B���O�̏ꍇ�ASJIS�֕ϊ�
			//JIS�̏ꍇ�AJIS��SJIS�ɕϊ�
			if( curEncoding == GetJISTextEncoding() )
			{
				//B0126
				ConvertFromJISToSJIS(ioText);
				curEncoding = GetSJISTextEncoding();
			}
			//EUC�̏ꍇ�AEUC��SJIS�ɕϊ� B0141
			if( curEncoding == GetEUCTextEncoding() )
			{
				ConvertFromEUCToSJIS(ioText);
				curEncoding = GetSJISTextEncoding();
			}
			
			//�BUTF16�֕ϊ�
			// �e�핶���R�[�h��UTF16
			if( curEncoding != GetUTF16TextEncoding() )
			{
				AText	buffer;
				if( TECConvertToUTF16(ioText,buffer,curEncoding,inUseFallback,outFallbackUsed) == false )   throw 0;
				ioText.SwapContent(buffer);
			}
			
			//#369 �g�ъG����
			*
			���̈ʒu�őΉ��\��
			*
			
			//�CUTF16��UTF8
			AText	buffer;
			if( TECConvertUTFx(ioText,buffer,GetUTF16TextEncoding(),GetUTF8TextEncoding()) == false )   throw 0;
			ioText.SwapContent(buffer);
			*/
			
		}
	}
	catch(...)
	{
		result = false;
	}
	
	//CFString���
	if( strref != NULL )
	{
		::CFRelease(strref);
	}
	if( mstrref != NULL )
	{
		::CFRelease(mstrref);
	}
	
	return result;
}

/**
UTF-8->�w��TextEncoding

ioText�̃e�L�X�g��UTF-8����inTextEncoding�ɕϊ����AioText�Ɋi�[����
�ϊ����s�̏ꍇ��false��Ԃ��B���̏ꍇ�AioText�̓��e�͔j�󂳂�Ă���B
*/
ABool	ATextEncodingWrapper::ConvertFromUTF8( AText& ioText, const ATextEncoding inDstTextEncoding, 
											   const ABool inConvertToCanonical )//#1044
{
	if( ioText.GetItemCount() == 0 )   return true;
	
	// 1. UTF-8�̏ꍇ�A����ŏI��
	if( inDstTextEncoding == GetUTF8TextEncoding() )
	{
		//NFC�`���ւ̎����ϊ��iUTF-8�ۑ��̏ꍇ�ł����ꂾ���͏�������j #1044
		if( inConvertToCanonical == true )
		{
			ConvertToCanonicalComp(ioText);
		}
		return true;
	}
	
	//#1040
	//���݂�UTF-8�e�L�X�g����CFString����
	AStCreateCFStringFromAText	str(ioText);
	
	ABool	result = true;
	CFMutableStringRef	mstrref = NULL;
	try 
	{
		//CFMutableString����
		mstrref = ::CFStringCreateMutableCopy(kCFAllocatorDefault,0,str.GetRef());
		if( mstrref == NULL )   throw 0;
		
		//JIS�n���ʂ̕ϊ�
		if( IsSJISSeriesTextEncoding(inDstTextEncoding) == true || 
			inDstTextEncoding == GetJISTextEncoding() || 
			inDstTextEncoding == GetEUCTextEncoding() )
		{
			//���̈���
			//��U+FFE0����SJIS(Mac), SJIS(0208)�ŕϊ��ł��Ȃ��̂�U+00A2�ɕϊ�����B
			//�iSJIS(Win)�ł�U+00A2�ł��ϊ�OK�j
			ConvertCFMutableStringChar(mstrref,0xFFE0,0x00A2);
			//���̈���
			//��U+FFE1����SJIS(Mac), SJIS(0208)�ŕϊ��ł��Ȃ��̂�U+00A3�ɕϊ�����B
			//�iSJIS(Win)�ł�U+00A3�ł��ϊ�OK�j
			ConvertCFMutableStringChar(mstrref,0xFFE1,0x00A3);
			//�|�i�S�p�n�C�t���}�C�i�X�j�̈���
			//��U+FF0D����SJIS(Mac), SJIS(0208)�ŕϊ��ł��Ȃ��̂�U+2212�ɕϊ�����B
			ConvertCFMutableStringChar(mstrref,0xFF0D,0x2212);
			//�a�i���s�L���j�̈���
			//��U+2225����SJIS(Mac), SJIS(0208)�ŕϊ��ł��Ȃ��̂�U+2016�ɕϊ�����B
			ConvertCFMutableStringChar(mstrref,0x2225,0x2016);
			//�`�i�g�_�b�V���j�̈���
			ConvertCFMutableStringChar(mstrref,0xFF5E,0x301C);
			//�ʁi�ے�L���j�̈���
			ConvertCFMutableStringChar(mstrref,0xFFE2,0x00AC);
			//�_�b�V��(SJIS:0x815C)�̈���
			//JIS�K�i�AMac�AVista�ȍ~�FU+2014
			//Unicode�R���\�[�V�A���ACP932�FU+2015
			//��U+2015����SJIS(Mac), SJIS(0208)�ŕϊ��ł��Ȃ��̂�U+2014�ɕϊ�����B
			//�iSJIS(Win)�ł�U+2014�ł��ϊ�OK�j
			ConvertCFMutableStringChar(mstrref,0x2015,0x2014);
		}
		//���p���A�o�b�N�X���b�V���̏���
		switch(inDstTextEncoding)
		{
			//���p��(U+00A5)�֒u��
		  case kCFStringEncodingMacJapanese:
		  case kCFStringEncodingShiftJIS:
		  case kCFStringEncodingShiftJIS_X0213:
		  case kCFStringEncodingShiftJIS_X0213_MenKuTen:
			{
				//SJIS(Mac), SJIS(0208)�ł�U+00A5��0x5C�AU+005C��0x80�Ƀ}�b�s���O�����
				//����āAU+005C��U+00A5�֒u������
				ConvertCFMutableStringChar(mstrref,0x005C,0x00A5);
				break;
			}
			//�o�b�N�X���b�V��(U+005C)�֒u��
		  case kCFStringEncodingDOSJapanese:
		  case kCFStringEncodingEUC_JP:
		  case kCFStringEncodingISO_2022_JP:
		  case kCFStringEncodingISO_2022_JP_1:
		  case kCFStringEncodingISO_2022_JP_2:
		  case kCFStringEncodingISO_2022_JP_3:
			{
				//SJIS(Win)�ł�U+00A5���ϊ��G���[�AU+005C��0x5C�Ƀ}�b�s���O�����
				//����āAU+00A5��U+005C�֒u������
				ConvertCFMutableStringChar(mstrref,0x00A5,0x005C);
				break;
			}
		}
		//���p�`���_�A�I�[�o�[���C���̏���
		switch(inDstTextEncoding)
		{
			//�I�[�o�[���C��(U+203E)�֒u��
		  case kCFStringEncodingShiftJIS:
		  case kCFStringEncodingShiftJIS_X0213:
		  case kCFStringEncodingShiftJIS_X0213_MenKuTen:
			{
				//SJIS(0208)�ł�U+007E���ϊ��G���[�AU+203E��0x7E�Ƀ}�b�s���O�����
				//����āAU+007E��U+203E�֒u������
				ConvertCFMutableStringChar(mstrref,0x007E,0x203E);
				break;
			}
			//���p�`���_(U+007E)�֒u��
		  case kCFStringEncodingMacJapanese:
		  case kCFStringEncodingDOSJapanese:
		  case kCFStringEncodingEUC_JP:
		  case kCFStringEncodingISO_2022_JP:
		  case kCFStringEncodingISO_2022_JP_1:
		  case kCFStringEncodingISO_2022_JP_2:
		  case kCFStringEncodingISO_2022_JP_3:
			{
				//SJIS(Mac), SJIS(Win)�ł�U+007E��0x7E�AU+203E���ϊ��G���[�Ƀ}�b�s���O�����
				//����āAU+203E��U+007E�֒u������
				ConvertCFMutableStringChar(mstrref,0x203E,0x007E);
				break;
			}
		}
		
		//Canonical Comp�`���ւ̕ϊ�
		if( inConvertToCanonical == true )
		{
			//Normalize���s
			::CFStringNormalize(mstrref,kCFStringNormalizationFormC);
		}
		
		//CFString�S�̂�range���擾
		CFRange	range = ::CFRangeMake(0,::CFStringGetLength(mstrref));
		
		//���݂�ioText��S�폜
		ioText.DeleteAll();
		
		//�ϊ���ɕK�v�ȃo�C�g�����擾
		//ioText.Reserve(0,::CFStringGetMaximumSizeForEncoding(::CFStringGetLength(str.GetRef())+1024,inDstTextEncoding));
		CFIndex	resultLen = 0;
		::CFStringGetBytes(mstrref,range,inDstTextEncoding,0,false,NULL,0,&resultLen);
		
		//�ϊ��㕶����i�[��m��
		ioText.Reserve(0,resultLen);
		
		//�w��e�L�X�g�G���R�[�f�B���O�̃o�C�g��擾�iBOM�͂����ł͕t���Ȃ��j
		{
			AStTextPtr	textptr(ioText,0,ioText.GetItemCount());
			CFIndex	index = ::CFStringGetBytes(mstrref,range,inDstTextEncoding,0,false,
											   textptr.GetPtr(), textptr.GetByteCount(), &resultLen );
			//�ϊ�������������CFString�̕������ƈ�v���Ă��Ȃ���΃G���[�Ƃ���
			if( index != ::CFStringGetLength(mstrref) )   throw 0;
		}
		//ioText���g�p�������폜�i���߂Ɋm�ۂ�����@�͎~�߂��̂ŕs�v�Ǝv�����A�O�̂��ߎc���j
		ioText.Delete(resultLen,ioText.GetItemCount()-resultLen);
	}
	catch(...) 
	{
		result = false;
	}
	if( mstrref != NULL )
	{
		::CFRelease(mstrref);
	}
	return result;
	
	/*#1040
	//UTF-8�ȊO�̏ꍇ�A��������UTF-16�ɕϊ�����ioText�Ɋi�[
	//B0000 ABool	result = true;
	{
		//��������UTF-16�֕ϊ�
		//�iinConvertToCanonical��true�̂Ƃ���UTF-16��Canonical Composition�ɕϊ�����j
		ATextEncoding	utf16tec = GetUTF16TextEncoding();
		if( inConvertToCanonical == true )
		{
			utf16tec = GetUTF16CanonicalCompTextEncoding();
		}
		//
		AText	buffer;
		if( TECConvertUTFx(ioText,buffer,GetUTF8TextEncoding(),utf16tec) == false )
		{
			return false;
		}
		ioText.SwapContent(buffer);
	}
	
	//JIS�n�̏ꍇ�A0x00A5��0x005C�֕ϊ�����i�ꉞ�ݒ�save5CForYenWhenJIS�͗p�ӂ��邪�Ƃ肠������ɕϊ��ɂ���j
	if( inDstTextEncoding == GetSJISTextEncoding() || inDstTextEncoding == GetJISTextEncoding() || inDstTextEncoding == GetEUCTextEncoding() )
	{
		AStTextPtr	textptr(ioText,0,ioText.GetItemCount());
		AUChar*	ptr = textptr.GetPtr();
		for( AIndex i = 0; i+1 < textptr.GetByteCount(); i += 2 )
		{
			AUTF16Char*	ptr_uc = (AUTF16Char*)&(ptr)[i];
			if( *ptr_uc == 0x00A5 )
			{
				*ptr_uc = 0x005C;
			}
			
#if IMPLEMENTATION_FOR_MACOSX
			//Mac OS X�p�ϊ��␳
			
			//Mac�̏ꍇ��U+FF5E(�S�p�`���_)�������JIS�n�ŕۑ��ł��Ȃ��@#200
			switch(*ptr_uc)
			{
			  case 0xFF0D:
				{
					*ptr_uc = 0x2212;//�|
					break;
				}
			  case 0xFF5E:
				{
					*ptr_uc = 0x301C;//�`
					break;
				}
			}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
			//Windows�p�ϊ��␳
			
			//Windows�̏ꍇ��U+301C(�g�_�b�V��)��JIS�n�ŕۑ��ł��Ȃ��i"?"�ɉ�����j
			switch(*ptr_uc)
			{
			  case 0x301C:
				{
					*ptr_uc = 0xFF5E;//�`
					break;
				}
			}
#endif
		}
	}
	
	// 2. UTF-16�̏ꍇ�A����ŏI��
	if( inDstTextEncoding == GetUTF16TextEncoding() )
	{
		return true;
	}
	// 3. UTF-16LE�̏ꍇ�APowerPCMac�Ȃ�Abyte order��ϊ����ďI��
	else if( inDstTextEncoding == GetUTF16LETextEncoding() )
	{
		if( UTF16IsLE() == false )
		{
			ChangeByteOrder(ioText);
		}
		return true;
	}
	//B0377 UTF-16BE�̏ꍇ�AIntelMac�Ȃ�Abyte order��ϊ����ďI��
	else if( inDstTextEncoding == GetUTF16BETextEncoding() )
	{
		if( UTF16IsLE() == true )
		{
			ChangeByteOrder(ioText);
		}
		return true;
	}
	// 4. JIS�̏ꍇ
	else if( inDstTextEncoding == GetJISTextEncoding() )
	{
		//UTF16->SJIS
		AText	buffer;
		if( TECConvertFromUTF16(ioText,buffer,GetSJISTextEncoding()) == false )
		{
			//�ϊ��G���[
			return false;
		}
		//SJIS->JIS
		//B0126
		{
			ConvertFromSJISToJIS(buffer,ioText);
		}
		return true;
	}
	// 5. EUC�̏ꍇ B0141
	else if( inDstTextEncoding == GetEUCTextEncoding() )
	{
		//UTF16->SJIS
		AText	buffer;
		if( TECConvertFromUTF16(ioText,buffer,GetSJISTextEncoding()) == false )
		{
			//�ϊ��G���[
			return false;
		}
		//SJIS->EUC
		{
			ConvertFromSJISToEUC(buffer,ioText);
		}
		return true;
	}
	// ����ȊO
	else
	{
		//UTF16->���ꂼ���TextEncoding
		AText	buffer;
		if( TECConvertFromUTF16(ioText,buffer,inDstTextEncoding) == false )
		{
			//�ϊ��G���[
			return false;
		}
		ioText.SwapContent(buffer);
		return true;
	}
	*/
}

//#1040
/**
CFMutableString�����u��
*/
void	ATextEncodingWrapper::ConvertCFMutableStringChar( CFMutableStringRef ioString ,const AUCS4Char inFromChar, const AUCS4Char inToChar )
{
	AText	fromCharText, toCharText;
	fromCharText.AddFromUCS4Char(inFromChar);
	toCharText.AddFromUCS4Char(inToChar);
	AStCreateCFStringFromAText	fromCharStr(fromCharText);
	AStCreateCFStringFromAText	toCharStr(toCharText);
	::CFStringFindAndReplace(ioString,fromCharStr.GetRef(),toCharStr.GetRef(),::CFRangeMake(0,::CFStringGetLength(ioString)),0);
}

//#1040
/**
SJIS�n�e�L�X�g�G���R�[�f�B���O���ǂ����𔻒�
*/
ABool	ATextEncodingWrapper::IsSJISSeriesTextEncoding( const ATextEncoding inTextEncoding )
{
	switch(inTextEncoding)
	{
	  case kCFStringEncodingMacJapanese:
	  case kCFStringEncodingShiftJIS_X0213:
	  case kCFStringEncodingShiftJIS_X0213_MenKuTen:
	  case kCFStringEncodingShiftJIS:
	  //case kCFStringEncodingShiftJIS_X0213_00:
	  case kCFStringEncodingDOSJapanese:
		{
			return true;
		}
	  default:
		{
			return false;
		}
	}
}

//#1040
/**
UTF-8��UTF-8(CanonicalComp)�`���֕ϊ�
CanonicalComp:�Ђ炪�ȁ{���_�A�A���t�@�x�b�g�{�E�����E�g�����ꕶ���ŕ\���W���`(NFC�`��)
*/
void	ATextEncodingWrapper::ConvertToCanonicalComp( AText& ioText )
{
#if IMPLEMENTATION_FOR_MACOSX
	/*#1040
	AText	buffer;
	TECConvertFromUTF16(ioUTF16Text,buffer,::CreateTextEncoding(kTextEncodingUnicodeDefault,kUnicodeCanonicalCompVariant,kUnicode16BitFormat));
	ioUTF16Text.SetText(buffer);
	*/
	//CFMutableString����
	AStCreateCFStringFromAText	str(ioText);
	CFMutableStringRef	mstrref = ::CFStringCreateMutableCopy(kCFAllocatorDefault,0,str.GetRef());
	if( mstrref != NULL )
	{
		//Normalize���s
		::CFStringNormalize(mstrref,kCFStringNormalizationFormC);
		//���ʐݒ�
		ioText.SetFromCFString(mstrref);
		//CFMutableString���
		::CFRelease(mstrref);
	}

#elif IMPLEMENTATION_FOR_WINDOWS
	/*Normaliz.dll���K�v��AUnicodeData::NormalizeD()���g��
	AStTextPtr	intextptr(ioUTF16Text,0,ioUTF16Text.GetItemCount());
	AText	buffer;
	buffer.Reserve(0,ioUTF16Text.GetItemCount()+1024);//NFC�ւ̕ϊ��Ȃ猳�̃e�L�X�g�T�C�Y���瑝���邱�Ƃ͂Ȃ����낤�̂�
	AStTextPtr	bufferptr(buffer,0,buffer.GetItemCount());
	int unilen = ::NormalizeString(NormalizationC,reinterpret_cast<LPCWSTR>(intextptr.GetPtr()),intextptr.GetByteCount()/sizeof(wchar_t),
				reinterpret_cast<LPWSTR>(bufferptr.GetPtr()),bufferptr.GetByteCount()/sizeof(wchar_t));
	buffer.GetText(0,unilen*sizeof(wchar_t),ioUTF16Text);
	*/
#else
	not implemented
#endif
}

/**
����UTF16��LittleEndian���ǂ�����Ԃ�
*/
ABool	ATextEncodingWrapper::UTF16IsLE()
{
#if IMPLEMENTATION_FOR_MACOSX
#if __LITTLE_ENDIAN__
	return true;
#else
	return false;
#endif
#elif IMPLEMENTATION_FOR_WINDOWS
	//Windows�̏ꍇ�͏��LE�ň���
	return true;
#endif
}

#if IMPLEMENTATION_FOR_MACOSX
/**
UTF-16��HFSPlusDecomp�`���֕ϊ�
*/
void	ATextEncodingWrapper::ConvertUTF16ToHFSPlusDecomp( AText& ioUTF16Text )
{
	AText	buffer;
	TECConvertFromUTF16(ioUTF16Text,buffer,::CreateTextEncoding(kTextEncodingUnicodeDefault,kUnicodeHFSPlusDecompVariant,kUnicode16BitFormat));
	ioUTF16Text.SetText(buffer);
}
#endif

#pragma mark ===========================

#pragma mark ---TextEncoding�Ǝ��ϊ����W���[������

/**
ByteOrder����ւ�
*/
void	ATextEncodingWrapper::ChangeByteOrder( AText& ioText )
{
	AStTextPtr	textptr(ioText,0,ioText.GetItemCount());
	AByteCount	bytecount = textptr.GetByteCount();//#598
	AUChar*	ptr = textptr.GetPtr();
	for( AIndex pos = 0; pos+1 < /*#598 ioText.GetItemCount()*/bytecount; pos += 2 )
	{
		AUChar	ch = (ptr)[pos+1];
		(ptr)[pos+1] = (ptr)[pos];
		(ptr)[pos] = ch;
	}
}

//B0126 TECConvertText���g����\���S�p�ɉ������肷��̂œƎ����[�`�����g�p����B
/**
JIS->SJIS
*/
void	ATextEncodingWrapper::ConvertFromJISToSJIS( AText& ioText )
{
	AIndex	dstpos = 0;
	{
		AStTextPtr	textptr(ioText,0,ioText.GetItemCount());
		AByteCount	bytecount = textptr.GetByteCount();//#598
		AUChar*	ptr = textptr.GetPtr();
		AUChar	ch,ch2;
		ABool	kanjiMode = false;
		ABool	hankakuKanaMode = false;//#325
		for( AIndex srcpos = 0; srcpos < /*#598 ioText.GetItemCount()*/bytecount; srcpos++ ) 
		{
			ch = (ptr)[srcpos];
			if( srcpos+1 < /*#598 ioText.GetItemCount()*/bytecount )
			{
				ch2= (ptr)[srcpos+1];
			}
			else
			{
				ch2 = '\r';
			}
			if( kanjiMode && ch >= 0x21 && ch2 != 13 && ch2 != 10 ) 
			{
				jis2sjis(ch,ch2);
				(ptr)[dstpos] = ch;
				(ptr)[dstpos+1] = ch2;
				dstpos += 2;
				srcpos++;
			}
			//���p�J�i���[�h #325 
			else if( hankakuKanaMode == true && ch >= 0x21 ) 
			{
				(ptr)[dstpos] = ch+0x80;
				dstpos++;
			}
			//
			else
			{
				if( ch == 0x1B ) 
				{
					if( ch2 == '$' ) 
					{
						ch = (ptr)[srcpos+2];
						if( ch == '@' || ch == 'B' )
						{
							kanjiMode = true;
							hankakuKanaMode = false;//#325
							srcpos += 2;
						}
					}
					else if( ch2 == '(' ) 
					{
						ch = (ptr)[srcpos+2];
						if( ch == 'H' || ch == 'J' || ch == 'B' ) 
						{
							kanjiMode = false;
							hankakuKanaMode = false;//#325
							srcpos += 2;
						}
						//���p�J�i���[�h�ֈڍs#325
						else if( ch == 'I' )
						{
							kanjiMode = false;
							hankakuKanaMode = true;
							srcpos += 2;
						}
					}
					else if( ch2 == 'K' ) 
					{
						kanjiMode = true;
						hankakuKanaMode = false;//#325
						srcpos++;
					}
					else if( ch2 == 'H' ) 
					{
						kanjiMode = false;
						hankakuKanaMode = false;//#325
						srcpos++;
					}
				}
				else
				{
					(ptr)[dstpos] = ch;
					dstpos++;
				}
			}
		}
	}
	ioText.Delete(dstpos,ioText.GetItemCount()-dstpos);
}

//B0141
/**
EUC->SJIS
*/
void	ATextEncodingWrapper::ConvertFromEUCToSJIS( AText& ioText )
{
	AIndex	dstpos = 0;
	{
		AStTextPtr	textptr(ioText,0,ioText.GetItemCount());
		AItemCount	bytecount = textptr.GetByteCount();//#598
		AUChar*	ptr = textptr.GetPtr();
		AUChar	ch,ch2;
		//B0000 ABool	kanjiMode = false;
		for( AIndex srcpos = 0; srcpos < /*#598 ioText.GetItemCount()*/bytecount; srcpos++ ) 
		{
			ch = (ptr)[srcpos];
			if( srcpos+1 < /*#598 ioText.GetItemCount()*/bytecount )
			{
				ch2= (ptr)[srcpos+1];
			}
			else
			{
				ch2 = '\r';
			}
			if( (ch & 0x80) && ch2 != 13 && ch2 != 10 )
			{
				if( ch == 0x8E )//���p�J�i�Ȃ�
				{
					(ptr)[dstpos] = ch2;
					dstpos++;
					srcpos++;
				}
				else
				{
					euc2sjis(ch,ch2);
					(ptr)[dstpos] = ch;
					(ptr)[dstpos+1] = ch2;
					dstpos += 2;
					srcpos++;
				}
			}
			else
			{
				(ptr)[dstpos] = ch;
				dstpos++;
			}
		}
	}
	ioText.Delete(dstpos,ioText.GetItemCount()-dstpos);
}

//B0126 TECConvertText���g����\���S�p�ɉ������肷��̂œƎ����[�`�����g�p����B
/**
SJIS-JIS
*/
void	ATextEncodingWrapper::ConvertFromSJISToJIS( const AText& inText, AText& outBuffer )
{
	//�o�͐�S�폜
	outBuffer.DeleteAll();
	
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	AByteCount	bytecount = textptr.GetByteCount();//#598
	AUChar*	ptr = textptr.GetPtr();
	AUChar ch1,ch2;
	ABool	kanjiMode = false;
	ABool	hankakuKanaMode = false;//#325
	for( AIndex pos = 0; pos < /*#598 inText.GetItemCount()*/bytecount; pos++ )
	{
		ch1 = (ptr)[pos];
		if( pos+1 < /*#598 inText.GetItemCount()*/bytecount )
		{
			ch2 = (ptr)[pos+1];
		}
		else
		{
			ch2 = '\r';
		}
		if( ch1 == '\r' || ch1 == 10/*B0199*/ ) 
		{
			if( kanjiMode || hankakuKanaMode ) //#325
			{
				outBuffer.Add(0x1B);
				outBuffer.Add('(');
				outBuffer.Add('B');
				kanjiMode = false;
				hankakuKanaMode = false;
			}
			outBuffer.Add(ch1);/*B0199*/
		}
		//���p�J�i #325
		else if( ch1>=0xA1 && ch1<=0xDF )
		{
			if( hankakuKanaMode == false )
			{
				outBuffer.Add(0x1B);
				outBuffer.Add('(');
				outBuffer.Add('I');
				kanjiMode = false;
				hankakuKanaMode = true;
			}
			outBuffer.Add(ch1-0x80);
		}
		//ASCII�������G�X�P�[�v����
		else if( !IsSJISKanjiChar(ch1) || ch2 == '\r' || ch2 == 10/*B0199*/ )
		{
			if( kanjiMode || hankakuKanaMode ) //#325
			{
				outBuffer.Add(0x1B);
				outBuffer.Add('(');
				outBuffer.Add('B');
				kanjiMode = false;
				hankakuKanaMode = false;
			}
			outBuffer.Add(ch1);
		}
		//����
		else
		{
			if( !kanjiMode ) 
			{
				outBuffer.Add(0x1B);
				outBuffer.Add('$');
				outBuffer.Add('B');
				kanjiMode = true;
				hankakuKanaMode = false;
			}
			sjis2jis(ch1,ch2);
			outBuffer.Add(ch1);
			outBuffer.Add(ch2);
			pos++;
		}
	}
}

//B0141
/**
SJIS->EUC
*/
void	ATextEncodingWrapper::ConvertFromSJISToEUC( const AText& inText, AText& outBuffer )
{
	//�o�͐�S�폜
	outBuffer.DeleteAll();
	
	AStTextPtr	textptr(inText,0,inText.GetItemCount());
	AByteCount	bytecount = textptr.GetByteCount();//#598
	AUChar*	ptr = textptr.GetPtr();
	unsigned char ch1,ch2;
	for( AIndex pos = 0; pos < /*#598 inText.GetItemCount()*/bytecount; pos++ )
	{
		ch1 = (ptr)[pos];
		if( pos+1 < /*#598 inText.GetItemCount()*/bytecount )
		{
			ch2 = (ptr)[pos+1];
		}
		else
		{
			ch2 = '\r';
		}
		if( !IsSJISKanjiChar(ch1) || ch2 == '\r' ) 
		{
			if( ch1 >= 0x80 )//���p�J�i�Ȃ�
			{
				outBuffer.Add(0x8E);
				outBuffer.Add(ch1);
			}
			else
			{
				outBuffer.Add(ch1);
			}
		}
		else 
		{
			sjis2euc(ch1,ch2);
			outBuffer.Add(ch1);
			outBuffer.Add(ch2);
			pos++;
		}
	}
}

/**
SJIS�ł̊����������ǂ������擾
*/
ABool	ATextEncodingWrapper::IsSJISKanjiChar( const AUChar c )
{
	return ((c>=0x81&&c<=0x9F)||(c>=0xE0&&c<=0xFC));
}

#pragma mark ===========================

#pragma mark ---TextEncoding�ϊ��p�������W���[��

// �e��G���R�[�f�B���O->UTF16
//B0126
/**
�w��TextEncoding->UTF-16
*/
ABool	ATextEncodingWrapper::TECConvertToUTF16( const AText& inText, AText& outText, 
		const ATextEncoding inSrc, ABool inUseFallback, ABool &outFallbackUsed )
{
#if IMPLEMENTATION_FOR_MACOSX
	//�o�͐�S�폜
	outText.DeleteAll();
	
	UnicodeMapping		exchangeMap;
	exchangeMap.otherEncoding = inSrc;
	exchangeMap.unicodeEncoding = GetUTF16TextEncoding();
	exchangeMap.mappingVersion = kUnicodeUseLatestMapping;
	//B0136
	TextToUnicodeInfo	textToUniInfo = NULL;
	ABool	result = true;
	try
	{
		//Convert��̈�m��
		outText.Reserve(0,inText.GetItemCount()*3 +1024);
		
		OSStatus	status;
		status = ::CreateTextToUnicodeInfo(&exchangeMap, &textToUniInfo);
		if( status != noErr )   throw 0;
		//B0168
		OptionBits	conversionFlag = kUnicodeForceASCIIRangeMask;
		if( inUseFallback )
		{
			conversionFlag |= kUnicodeUseFallbacksMask;
		}
		outFallbackUsed = false;
		
		AByteCount	readLen = 0,resultLen = 0;
		{
			AStTextPtr	intextptr(inText,0,inText.GetItemCount());
			AStTextPtr	outtextptr(outText,0,outText.GetItemCount());
			status = ::ConvertFromTextToUnicode(textToUniInfo,intextptr.GetByteCount(),intextptr.GetPtr(),conversionFlag,
					0,NULL,NULL,NULL,outtextptr.GetByteCount(),&readLen,&resultLen,(UniChar*)(outtextptr.GetPtr()));
		}
		//B0168
		if( status == kTECUsedFallbacksStatus )
		{
			outFallbackUsed = true;
		}
		else if( status != noErr )   throw 0;
		
		//outText���g�p�������폜
		outText.Delete(resultLen,outText.GetItemCount()-resultLen);
	}
	catch(...)//B0136
	{
		outText.DeleteAll();//B0000
		result = false;
	}
	if( textToUniInfo != NULL )   ::DisposeTextToUnicodeInfo(&textToUniInfo);//B0136
	return result;
#elif IMPLEMENTATION_FOR_WINDOWS
	//�o�͐�S�폜
	outText.DeleteAll();
	outFallbackUsed = false;
	if( inText.GetItemCount() == 0 )
	{
		return true;
	}
	
	ABool	result = true;
	try
	{
		int	unilen = 0;
		{
			AStTextPtr	intextptr(inText,0,inText.GetItemCount());
			//Convert��T�C�Y�v�Z�E�������m��
			unilen = ::MultiByteToWideChar(inSrc,MB_ERR_INVALID_CHARS,//MB_ERR_INVALID_CHARS���Ȃ��ꍇ�A�G���[�����������Ă��������ϊ������
					reinterpret_cast<LPCSTR>(intextptr.GetPtr()),intextptr.GetByteCount(),
					NULL,0);
			outText.Reserve(0,unilen*sizeof(wchar_t)+1024);
			
			AStTextPtr	outtextptr(outText,0,outText.GetItemCount());
			//Convert���s
			unilen = ::MultiByteToWideChar(inSrc,MB_ERR_INVALID_CHARS,//MB_ERR_INVALID_CHARS���Ȃ��ꍇ�A�G���[�����������Ă��������ϊ������
					reinterpret_cast<LPCSTR>(intextptr.GetPtr()),intextptr.GetByteCount(),
					reinterpret_cast<LPWSTR>(outtextptr.GetPtr()),outtextptr.GetByteCount()/sizeof(AUTF16Char));
		}
		DWORD	err = GetLastError();
		if( unilen == 0 )
		{
			//_ACThrow("MultiByteToWideChar error",NULL);
			//CP�ԍ����Ή��ȂǁB52932�����G���[�ƂȂ�B
			return false;
		}
		
		AByteCount	resultLen = unilen*sizeof(wchar_t);
		//outText���g�p�������폜
		outText.Delete(resultLen,outText.GetItemCount()-resultLen);
	}
	catch(...)//B0136
	{
		outText.DeleteAll();//B0000
		result = false;
	}
	return result;
#else
	not implemented
#endif
}

// UTF16->�e��G���R�[�f�B���O
// �Ȃ���UTF16->JIS���ł��Ȃ�
//�����K�v���@�������fallback�I�v�V�����͕K�v�Ȃ��̂��i�����炱���K�v�ł͂Ȃ��̂��j
/**
UTF-16->�w��TextEncoding
*/
ABool	ATextEncodingWrapper::TECConvertFromUTF16( const AText& inText, AText& outText, const TextEncoding inDst )
{
#if IMPLEMENTATION_FOR_MACOSX
	//�o�͐�S�폜
	outText.DeleteAll();
	
	UnicodeMapping		exchangeMap;
	exchangeMap.otherEncoding = inDst;
	exchangeMap.unicodeEncoding = ::CreateTextEncoding(kTextEncodingUnicodeDefault,kUnicodeNoSubset,kUnicode16BitFormat);//#1040 GetUTF16TextEncoding();
	exchangeMap.mappingVersion = kUnicodeUseLatestMapping;
	//B0136
	UnicodeToTextInfo	uniToTextInfo = NULL;
	ABool	result = true;
	try
	{
		//Convert��̈�m��
		outText.Reserve(0,inText.GetItemCount()*3 +1024);
		
		OSStatus	status;
		status = ::CreateUnicodeToTextInfo(&exchangeMap, &uniToTextInfo);
		if( status != noErr )   throw 0;
		ByteCount	readLen,resultLen;
		{
			AStTextPtr	intextptr(inText,0,inText.GetItemCount());
			AStTextPtr	outtextptr(outText,0,outText.GetItemCount());
			status = ::ConvertFromUnicodeToText(uniToTextInfo,intextptr.GetByteCount(),(UniChar*)(intextptr.GetPtr()),
					kUnicodeForceASCIIRangeMask,//conversion flag
					0,NULL,NULL,NULL,outtextptr.GetByteCount(),&readLen,&resultLen,outtextptr.GetPtr());
			if( status != noErr )   throw 0;
		}
		
		//outText���g�p�������폜
		outText.Delete(resultLen,outText.GetItemCount()-resultLen);
	}
	catch(...)//B0136
	{
		outText.DeleteAll();//B0000
		result = false;
	}
	if( uniToTextInfo != NULL )   ::DisposeUnicodeToTextInfo(&uniToTextInfo);//B0136
	return result;
#elif IMPLEMENTATION_FOR_WINDOWS
	//�o�͐�S�폜
	outText.DeleteAll();
	
	ABool	result = true;
	try
	{
		int	bytelen = 0;
		{
			AStTextPtr	intextptr(inText,0,inText.GetItemCount());
			//Convert��T�C�Y�v�Z�E�������m��
			bytelen = ::WideCharToMultiByte(inDst,0,
					reinterpret_cast<LPCWSTR>(intextptr.GetPtr()),intextptr.GetByteCount()/sizeof(AUTF16Char),
					NULL,0,
					NULL,NULL);//default char�̓f�t�H���g
			outText.Reserve(0,bytelen+1024);
			
			AStTextPtr	outtextptr(outText,0,outText.GetItemCount());
			//Convert���s
			bytelen = ::WideCharToMultiByte(inDst,0,
					reinterpret_cast<LPCWSTR>(intextptr.GetPtr()),intextptr.GetByteCount()/sizeof(AUTF16Char),
					reinterpret_cast<LPSTR>(outtextptr.GetPtr()),outtextptr.GetByteCount(),
					NULL,NULL);
		}
		
		//outText���g�p�������폜
		outText.Delete(bytelen,outText.GetItemCount()-bytelen);
	}
	catch(...)
	{
			outText.DeleteAll();
			result = false;
	}
	return result;
#else
	not implemented
#endif
}

// TECConvertText�ł͕ϊ��s�\�����������?�ɕϊ������
// ~��\������ɕʂ̕����ɕϊ����ꂽ�肷��̂ŁAUTF8<->UTF16�݂̂Ɏg�p����
//�ϊ��G���[�̏ꍇ��false��Ԃ��AioText�͕ύX����Ȃ�
/**
UTF8<->UTF16�ϊ�
*/
ABool	ATextEncodingWrapper::TECConvertUTFx( const AText& inText, AText& outText, const ATextEncoding inSrc, const ATextEncoding inDst )
{
#if IMPLEMENTATION_FOR_MACOSX
	//�o�͐�S�폜
	outText.DeleteAll();
	
	TECObjectRef	converter = NULL;//B0136
	ABool	converterCreated = false;//R0108
	ABool	result = true;
	try
	{
		//Convert��̈�m��
		outText.Reserve(0,inText.GetItemCount()*3 +1024);
		
		OSStatus	status;
		//R0108 �\�Ȍ���mTextEncodingPref�ł��炩���ߍ쐬����converter���g�p����i�������j
		//�i���C���X���b�h�̏ꍇ�̂݁Bconverter���X���b�h�Ԃŋ��L�ł��Ȃ��B
		if( gBase != NULL )
		{
			if( gBase->InMainThread() == true )
			{
				converter = GetConverter(inSrc,inDst);
			}
		}
		if( converter == NULL )
		{
			status = ::TECCreateConverter(&converter,inSrc,inDst);
			if( status != noErr )   throw 0;
			converterCreated = true;
		}
		//status = ::TECSetBasicOptions(converter,kUnicodeForceASCIIRangeMask);
		//if( status != noErr )   throw 0;
		ByteCount	readLen,resultLen;
		{
			AStTextPtr	intextptr(inText,0,inText.GetItemCount());
			AStTextPtr	outtextptr(outText,0,outText.GetItemCount());
			status = ::TECConvertText(converter,intextptr.GetPtr(),intextptr.GetByteCount(),&readLen,
					outtextptr.GetPtr(),outtextptr.GetByteCount(),&resultLen);
			if( status != noErr )   throw 0;
		}
		
		//outText���g�p�������폜
		outText.Delete(resultLen,outText.GetItemCount()-resultLen);
	}
	catch(...)//B0136
	{
		result = false;
	}
	//�t���b�V��
	unsigned char	buffer[4096];
	ByteCount	actual = 0;
	::TECFlushText(converter,buffer,4095,&actual);
	if( converterCreated && (converter != NULL) )   ::TECDisposeConverter(converter);//B0136 converter����R��C��
	return result;
#elif IMPLEMENTATION_FOR_WINDOWS
	if( inDst == GetUTF16TextEncoding() )
	{
		ABool	fallback;
		return TECConvertToUTF16(inText,outText,inSrc,false,fallback);
	}
	else if( inSrc == GetUTF16TextEncoding() )
	{
		return TECConvertFromUTF16(inText,outText,inDst);
	}
	else
	{
		_ACError("",NULL);
	}
	return false;
#else
	not implemented
#endif
}

/**
JIS����->SJIS����
*/
void	ATextEncodingWrapper::jis2sjis( unsigned char &a, unsigned char &b )
{
	if( a & 1 ) 
	{
		if( b < 0x60 )	b += 0x1f;
		else			b += 0x20;
	} 
	else   				b += 0x7e;
	if ( a < 0x5f )		a = ( a + 0xe1 ) >> 1;
	else				a = ( a + 0x161 ) >> 1;
}

/**
EUC����->JIS����
*/
void	ATextEncodingWrapper::euc2sjis( unsigned char &a, unsigned char &b )
{
	a &= 0x7f;
	b &= 0x7f;
	
	jis2sjis( a, b );
}

/**
SJIS����->JIS����
*/
void	ATextEncodingWrapper::sjis2jis( unsigned char &a, unsigned char &b )
{
	if ( a <= 0x9f ) 
	{
		if	( b < 0x9f )	a = ( a << 1 ) - 0xe1;
		else				a = ( a << 1 ) - 0xe0;
	} 
	else 
	{
		if	( b < 0x9f )	a = ( a << 1 ) - 0x161;
		else				a = ( a << 1 ) - 0x160;
	}
	if		( b < 0x7f )	b -= 0x1f;
	else if	( b < 0x9f )	b -= 0x20;
	else					b -= 0x7e;
}

/**
SJIS����->EUC����
*/
void	ATextEncodingWrapper::sjis2euc( unsigned char &a, unsigned char &b )
{
	sjis2jis( a, b );
	a |= 0x80;
	b |= 0x80;
}

#pragma mark ===========================

#pragma mark ---TextEncoding�擾

/**
IANA���̂���ATextEncoding���擾
@note ������Ȃ��ꍇ��UTF8��Ԃ�
*/
ATextEncoding	ATextEncodingWrapper::GetTextEncodingFromName( const AText& inName )
{
	ATextEncoding	tec = ATextEncodingWrapper::GetUTF8TextEncoding();
	if( GetTextEncodingFromName(inName,tec) == false )
	{
		tec = ATextEncodingWrapper::GetUTF8TextEncoding();
	}
	return tec;
}

/**
IANA���̂���ATextEncoding���擾
*/
ABool	ATextEncodingWrapper::GetTextEncodingFromName( AConstCharPtr inName, ATextEncoding& outEncoding )
{
	AText	text(inName);
	return GetTextEncodingFromName(text,outEncoding);
}

/**
mi�ݒ薼�̂���ATextEncoding���擾
*/
ABool	ATextEncodingWrapper::GetTextEncodingFromName( const AText& inName, ATextEncoding& outEncoding )
{
	//--------------------Shift_JIS�p���ꏈ��-------------------- 
	//�]����TEC Manager�ł�Shift_JIS�ł̕ϊ���CP932�x�[�X���������Ƃ���A
	//2.1, 3.0.0b5�ȑO��Shift_JIS�ŕۑ������CP932�x�[�X�ŕۑ�����Ă���B
	//���̂��߁A���L�̂悤�ɂ���B
	//DocPref�ɐV�ݒ�kTextEncodingNameCocoa��ǉ�����B
	//1. ���ݒ�"Shift_JIS"�E�V�ݒ�""�̏ꍇ�F�����ɂ�"Shift_JIS"�ŗ��遨kCFStringEncodingDOSJapanese(=Windows-31J)�Ƃ��Ĉ���
	// �ۑ����́A���ݒ�"Shift_JIS"�A�V�ݒ�"Shift_JIS"
	//2. ���ݒ�"Shift_JIS"�E�V�ݒ�"Shift_JIS"�̏ꍇ�F�����ɂ�"Shift_JIS"�ŗ��遨kCFStringEncodingDOSJapanese(=Windows-31J)�Ƃ��Ĉ���
	// �ۑ����́A���ݒ�"Shift_JIS"�A�V�ݒ�"Shift_JIS"
	//3. ���ݒ�"Shift_JIS"�E�V�ݒ�"Shift_JIS_X_0208"�̏ꍇ�F�����ɂ�"Shift_JIS_X_0208"�ŗ��遨kCFStringEncodingShiftJIS(=�W��JIS)�Ƃ��Ĉ���
	// �ۑ����́A���ݒ�"Shift_JIS"�A�V�ݒ�"Shift_JIS_X_0208"
	if( inName.CompareIgnoreCase("Shift_JIS") == true )
	{
		outEncoding = kCFStringEncodingDOSJapanese;//Windows-31J�ɑΉ�
		return true;
	}
	if( inName.CompareIgnoreCase("Shift_JIS_X_0208") == true )
	{
		outEncoding = kCFStringEncodingShiftJIS;
		return true;
	}
	
	//IANA���̂���ATextEncoding���擾
	return GetTextEncodingFromIANAName(inName,outEncoding);
}

//#1040
/**
IANA���̂���ATextEncoding���擾
*/
ABool	ATextEncodingWrapper::GetTextEncodingFromIANAName( const AText& inName, ATextEncoding& outEncoding )
{
#if IMPLEMENTATION_FOR_MACOSX
	//Shift_JISX0213��OS10.9�ł�API�ł͎擾�ł��Ȃ������̂ŁA�]�������Ɠ��l�ɁA���L3�͎����Őݒ�B
	if( inName.CompareIgnoreCase("Shift_JISX0213") == true )
	{
		outEncoding = kCFStringEncodingShiftJIS_X0213;
		return true;
	}
	if( inName.CompareIgnoreCase("UTF-16") == true )
	{
		outEncoding = kCFStringEncodingUTF16;
		return true;
	}
	if( inName.CompareIgnoreCase("UTF-8") == true )
	{
		outEncoding = kCFStringEncodingUTF8;
		return true;
	}
	
	//--------------------IANA������擾--------------------
	AStCreateCFStringFromAText	name(inName);
    outEncoding = ::CFStringConvertIANACharSetNameToEncoding(name.GetRef());
	if( outEncoding != kCFStringEncodingInvalidId )
	{
		return true;
	}
	else
	{
		//#372 <>���̐��l�Ŏw��
		AIndex	pos = 0;
		if( inName.FindCstring(0,"<",pos) == true )
		{
			pos++;
			ANumber	num = 0;
			if( inName.ParseIntegerNumber(pos,num,false) == true )
			{
				outEncoding = num;
				return true;
			}
		}
		return false;
	}
	/*#1040
	//OS�o�[�W�����ɂ���Ă͂�������Encoding��TECGetTextEncodingFromInternetName�ł͎擾�ł��Ȃ��̂�CreateTextEncoding�Ŏ擾����B
	if( inName.Compare("Shift_JISX0213") == true )
	{
		outEncoding = ::CreateTextEncoding(kTextEncodingShiftJIS_X0213_00,kTextEncodingDefaultVariant,kTextEncodingDefaultFormat);
		return true;
	}
	if( inName.Compare("UTF-16") == true )
	{
		outEncoding = ::CreateTextEncoding(kTextEncodingUnicodeDefault,kTextEncodingDefaultVariant,kUnicode16BitFormat);
		return true;
	}
	if( inName.Compare("UTF-8") == true )
	{
		outEncoding = ::CreateTextEncoding(kTextEncodingUnicodeDefault,kTextEncodingDefaultVariant,kUnicodeUTF8Format);
		return true;
	}
	else
	{
		Str255	name;
		inName.GetPstring(name,255);
		if( ::TECGetTextEncodingFromInternetName(&outEncoding,name) == noErr )   return true;
		//#372 <>���̐��l�Ŏw��
		AIndex	pos = 0;
		if( inName.FindCstring(0,"<",pos) == true )
		{
			pos++;
			ANumber	num = 0;
			if( inName.ParseIntegerNumber(pos,num,false) == true )
			{
				outEncoding = num;
				return true;
			}
		}
		return false;
	}
	*/
#elif IMPLEMENTATION_FOR_WINDOWS
	outEncoding = 932;//�Ƃ肠����SJIS
	AIndex	index = sTextEncodingArray_Name.Find(inName);
	if( index == kIndex_Invalid )
	{
		//<>����CP�ԍ��Ƃ݂Ȃ�
		AIndex	pos = 0;
		if( inName.FindCstring(0,"<",pos) == true )
		{
			pos++;
			ANumber	cpnum = 0;
			if( inName.ParseIntegerNumber(pos,cpnum,false) == true )
			{
				outEncoding = cpnum;
				return true;
			}
		}
		return false;
	}
	else
	{
		outEncoding = sTextEncodingArray_CodePage.Get(index);
		return true;
	}
#else
	not implemented
#endif
}

/**
ATextEncoding����mi�ݒ�p���̂��擾
*/
ABool	ATextEncodingWrapper::GetTextEncodingName( const ATextEncoding& inEncoding, AText& outName )
{
	outName.DeleteAll();
	//Shift_JIS�p���ꏈ��
	//ATextEncoding�FkCFStringEncodingDOSJapanese�́A�ݒ�ł̖���"Shift_JIS"�ɑΉ�
	//ATextEncoding�FkCFStringEncodingShiftJIS�́A�ݒ�ł̖���"Shift_JIS_X_0208"�ɑΉ�
	if( inEncoding == kCFStringEncodingDOSJapanese )
	{
		outName.SetCstring("Shift_JIS");
		return true;
	}
	if( inEncoding == kCFStringEncodingShiftJIS )
	{
		outName.SetCstring("Shift_JIS_X_0208");
		return true;
	}
	//ATextEncoding���炻��IANA���̂��擾
	return GetTextEncodingIANAName(inEncoding,outName);
}

//#1040
/**
ATextEncoding���炻��IANA���̂��擾
*/
ABool	ATextEncodingWrapper::GetTextEncodingIANAName( const ATextEncoding& inEncoding, AText& outName )
{
#if IMPLEMENTATION_FOR_MACOSX
	//OS�o�[�W�����ɂ���Ă�TECGetTextEncodingInternetName�Ŏ擾�ł��Ȃ������炵���̂ō�������ڐݒ�B
	if( inEncoding == kCFStringEncodingShiftJIS_X0213 )
	{
		outName.SetCstring("Shift_JISX0213");
		return true;
	}
	if( inEncoding == kCFStringEncodingUTF16 )
	{
		outName.SetCstring("UTF-16");
		return true;
	}
	if( inEncoding == kCFStringEncodingUTF8 )
	{
		outName.SetCstring("UTF-8");
		return true;
	}
	if( inEncoding == kCFStringEncodingMacJapanese )
	{
		outName.SetCstring("x-mac-japanese");
		return true;
	}
	//OS��API�ɂ��IANA���擾
	CFStringRef	strref = ::CFStringConvertEncodingToIANACharSetName(inEncoding);
	if( strref != NULL )
	{
		outName.SetFromCFString(strref);
		//::CFRelease(strref);
		return true;
	}
	else
	{
		return false;
	}
	/*#1040
	//OS�o�[�W�����ɂ���Ă�TECGetTextEncodingInternetName�Ŏ擾�ł��Ȃ��̂Œ��ڐݒ�B
	if( inEncoding == GetSJISX0213TextEncoding() )
	{
		outName.SetCstring("Shift_JISX0213");
	}
	else if( inEncoding == GetUTF16TextEncoding() )//B0126
	{
		outName.SetCstring("UTF-16");
	}
	else if( inEncoding == GetUTF8TextEncoding() )//B0126
	{
		outName.SetCstring("UTF-8");
	}
	else if( ::GetTextEncodingBase(inEncoding) == kTextEncodingMacJapanese )//#368
	{
		outName.SetCstring("x-mac-japanese");
	}
	else
	{
		Str255	name;
		if( ::TECGetTextEncodingInternetName(inEncoding,name) != noErr )
		{
			return false;
		}
		outName.SetPstring(name);
	}
	return true;
	*/
#elif IMPLEMENTATION_FOR_WINDOWS
	AIndex	index = sTextEncodingArray_CodePage.Find(inEncoding);
	if( index == kIndex_Invalid )
	{
		//��Windows�ɂ�CP���疼�O�擾����API�͂Ȃ�����
		outName.SetFormattedCstring("<%d>",inEncoding);
		return true;
	}
	else
	{
		sTextEncodingArray_Name.Get(index,outName);
		return true;
	}
#else
	not implemented
#endif
}

/**
�w�薼�̂�TextEncoding�����ݓ��쒆��OS�Ŏg�p�\���ǂ������`�F�b�N����
*/
ABool	ATextEncodingWrapper::CheckTextEncodingAvailability( const AText& inName )
{
#if IMPLEMENTATION_FOR_MACOSX
	ATextEncoding	enc;
	if( GetTextEncodingFromName(inName,enc) == true )
	{
		//���p�X�y�[�X��UTF8����ϊ���UTF8�֕ϊ��ł��邩�ǂ����������B
		AText	text(" ");
		if( ConvertFromUTF8(text,enc,false) == true )
		{
			ABool	fallback = false;
			if( ConvertToUTF8(text,enc,false,fallback,false) == true )
			{
				return true;
			}
		}
		/*#1040
		// JIS, SJIS, UTF8, UTF16, UTF16LE��ConvertFromTextToUnicode���g��Ȃ��̂Ŗ�����OK
		if( enc == GetJISTextEncoding() || enc == GetUTF8TextEncoding() ||
				enc == GetEUCTextEncoding() || enc == GetUTF16LETextEncoding() || 
			enc == GetUTF16TextEncoding() || enc == GetUTF16BETextEncoding() )
		{
			return true;
		}
		// UTF-16��enc, enc��UTF-16�̕ϊ����������邩����
		AText	text,text2;
		text.Add(0x00);
		text.Add(0x20);
		ABool	fallback;
		if( TECConvertToUTF16(text,text2,enc,true,fallback) == true )
		{
			if( TECConvertFromUTF16(text2,text,enc) == true )
			{
				return true;
			}
		}
		*/
	}
	return false;
#else
	ATextEncoding	enc;
	if( GetTextEncodingFromName(inName,enc) == true )
	{
		// JIS, SJIS, EUC, UTF-8, UTF-16�͖�����OK
		if( enc == GetSJISTextEncoding() || enc == GetJISTextEncoding() || enc == GetEUCTextEncoding() ||
					enc == GetUTF8TextEncoding() ||
					enc == GetUTF16TextEncoding() || 
					enc == GetUTF16LETextEncoding() || enc == GetUTF16BETextEncoding() )
		{
			return true;
		}
		// UTF-16��enc, enc��UTF-16�̕ϊ����������邩����
		AText	text,text2;
		text.Add(0x00);
		text.Add(0x20);
		ABool	fallback;
		if( TECConvertToUTF16(text,text2,enc,true,fallback) == true )
		{
			if( TECConvertFromUTF16(text2,text,enc) == true )
			{
				return true;
			}
		}
	}
	return false;
#endif
}

#if IMPLEMENTATION_FOR_MACOSX
/**
Converter�擾
*/
TECObjectRef	ATextEncodingWrapper::GetConverter( const ATextEncoding& inSrc, const ATextEncoding& inDst )
{
	if( inSrc == sUTF8TextEncoding && inDst == sUTF16TextEncoding )
	{
		return sConverterFromUTF8ToUTF16;
	}
	if( inSrc == sUTF16TextEncoding && inDst == sUTF8TextEncoding )
	{
		return sConverterFromUTF16ToUTF8;
	}
	return NULL;
}
#endif

//#1040
/**
�e�L�X�g�G���R�[�f�B���O�ݒ薼�̂�Carbon�ŗp�ݒ薼�̂֕ϊ�
*/
void ATextEncodingWrapper::ConvertToCarbonTextEncodingName( AText& ioText ) 
{
	if( ioText.Compare("Shift_JIS_X_0208") == true )
	{
		ioText.SetCstring("Shift_JIS");
	}
}

/**
JIS TextEncoding�擾
*/
ATextEncoding ATextEncodingWrapper::GetJISTextEncoding()
{
#if IMPLEMENTATION_FOR_MACOSX
	return sJISTextEncoding;
#elif IMPLEMENTATION_FOR_WINDOWS
	return 50222;
#else
	not implemented
#endif
}

/**
EUC TextEncoding�擾
*/
ATextEncoding ATextEncodingWrapper::GetEUCTextEncoding()
{
#if IMPLEMENTATION_FOR_MACOSX
	return sEUCTextEncoding;
#elif IMPLEMENTATION_FOR_WINDOWS
	return 51932;
#else
	not implemented
#endif
}

/**
SJIS TextEncoding�擾
*/
ATextEncoding ATextEncodingWrapper::GetSJISTextEncoding()
{
#if IMPLEMENTATION_FOR_MACOSX
	return sSJISTextEncoding;
#elif IMPLEMENTATION_FOR_WINDOWS
	return 932;
#else
	not implemented
#endif
}

/**
SJIS-X-0213 TextEncoding�擾
*/
ATextEncoding ATextEncodingWrapper::GetSJISX0213TextEncoding()
{
#if IMPLEMENTATION_FOR_MACOSX
	return sSJISX0213TextEncoding;
#elif IMPLEMENTATION_FOR_WINDOWS
	//��Windows��SJIS 0213���Ή��Ǝv����B932�͊G�����i�O���H�j�G���R�[�f�B���O�ɂȂ�
//	_ACError("not implemented",NULL);
	return 932;
#else
	not implemented
#endif
}

/**
UTF-8 TextEncoding�擾
*/
ATextEncoding ATextEncodingWrapper::GetUTF8TextEncoding()
{
#if IMPLEMENTATION_FOR_MACOSX
	return sUTF8TextEncoding;
#elif IMPLEMENTATION_FOR_WINDOWS
	return CP_UTF8;//UTF-8
#else
	not implemented
#endif
}

/**
UTF-16 TextEncoding�擾
*/
ATextEncoding ATextEncodingWrapper::GetUTF16TextEncoding()
{
#if IMPLEMENTATION_FOR_MACOSX
	return sUTF16TextEncoding;
#elif IMPLEMENTATION_FOR_WINDOWS
	return 1200;//UTF-16LE
#else
	not implemented
#endif
}
	
//#1044
/**
UTF-16(Canonical Composition) TextEncoding�擾
*/
/*#1040
ATextEncoding ATextEncodingWrapper::GetUTF16CanonicalCompTextEncoding()
{
#if IMPLEMENTATION_FOR_MACOSX
	return sUTF16CanonicalCompTextEncoding;
#elif IMPLEMENTATION_FOR_WINDOWS
	return 1200;//UTF-16LE
#else
	not implemented
#endif
}
*/

/**
UTF-16LE TextEncoding�擾
*/
ATextEncoding ATextEncodingWrapper::GetUTF16LETextEncoding()
{
#if IMPLEMENTATION_FOR_MACOSX
	return sUTF16LETextEncoding;
#elif IMPLEMENTATION_FOR_WINDOWS
	return 1200;//UTF-16LE
#else
	not implemented
#endif
}

//B0377
/**
UTF-16BE TextEncoding�擾
*/
ATextEncoding ATextEncodingWrapper::GetUTF16BETextEncoding()
{
#if IMPLEMENTATION_FOR_MACOSX
	return sUTF16BETextEncoding;
#elif IMPLEMENTATION_FOR_WINDOWS
	return 1201;//UTF-16BE
#else
	not implemented
#endif
}

//#427
/**
OS�f�t�H���g��TextEncoding�擾
*/
ATextEncoding	ATextEncodingWrapper::GetOSDefaultTextEncoding()
{
#if IMPLEMENTATION_FOR_MACOSX
	return ATextEncodingWrapper::GetUTF8TextEncoding();
#elif IMPLEMENTATION_FOR_WINDOWS
	return ::GetACP();
#else
	not implemented
#endif
}

#if 0
/**
�t�H���g������Legacy�e�L�X�g�G���R�[�f�B���O���擾
*/
ATextEncoding ATextEncodingWrapper::GetLegacyTextEncodingFromFont( /*win const AFont inFont*/const AText& inFontName )
{
#if IMPLEMENTATION_FOR_MACOSX
	SInt16	font;//win
	AFontWrapper::GetFontByName(inFontName,font);//win
	ATextEncoding	result;
	::UpgradeScriptInfoToTextEncoding(::FontToScript(font),kTextLanguageDontCare,kTextRegionDontCare,NULL,&result);
	return result;
#elif IMPLEMENTATION_FOR_WINDOWS
	//���b��
	return 932;
#else
	not implemented
#endif
}

//#189
/**
�e�����Legacy�e�L�X�g�G���R�[�f�B���O���擾
*/
ATextEncoding	ATextEncodingWrapper::GetLegacyTextEncodingFromLanguage( const ALanguage inLanguage )
{
#if IMPLEMENTATION_FOR_MACOSX
	ATextEncoding	result;
	switch(inLanguage)
	{
	  case kLanguage_Japanese:
		{
			result = kCFStringEncodingMacJapanese;//#1040 ::CreateTextEncoding(kTextEncodingMacJapanese,kTextEncodingDefaultVariant,kTextEncodingDefaultFormat);
			break;
		}
	  case kLanguage_English:
	  default:
		{
			result = kCFStringEncodingMacRoman;//#1040 ::CreateTextEncoding(kTextEncodingMacRoman,kTextEncodingDefaultVariant,kTextEncodingDefaultFormat);
			break;
		}
	}
	return result;
#elif IMPLEMENTATION_FOR_WINDOWS
	//���b��
	return 932;
#else
	not implemented
#endif
}
#endif

//#208
/**
JIS X 208�Œ�`���ꂽ�������ǂ������`�F�b�N����
*/
void	ATextEncodingWrapper::CheckJISX0208( const ATextEncoding inTextEncoding, const AText& inText, ABool& outKishuIzon, ABool& outHankakuKana )
{
	//�Q�l�Fhttp://charset.7jp.net/
	//�Q�l�Fhttp://www.asahi-net.or.jp/~ax2s-kmtn/index.html
	
	//�Ԃ�l������
	outKishuIzon = false;
	outHankakuKana = false;
	//SJIS�֕ϊ�
	AText	text;
	text.SetText(inText);
	if( inTextEncoding == ATextEncodingWrapper::GetJISTextEncoding() )
	{
		ATextEncodingWrapper::ConvertFromJISToSJIS(text);
	}
	else if( inTextEncoding == ATextEncodingWrapper::GetEUCTextEncoding() )
	{
		ATextEncodingWrapper::ConvertFromEUCToSJIS(text);
	}
	else if( ATextEncodingWrapper::IsSJISSeriesTextEncoding(inTextEncoding) == true )//#1040
	{
		//��������
	}
	else return;//JIS, SJIS, EUC�ȊO�̓`�F�b�N���Ȃ�
	//1�o�C�g�����`�F�b�N
	if( text.GetItemCount() == 1 )
	{
		AUChar	ch = text.Get(0);
		if( ch >= 0xA1 && ch <= 0xDF )
		{
			//���p�J�i
			outHankakuKana = true;
			return;
		}
		else if( (ch >= 0x80 && ch <= 0xA0) || (ch >= 0xE0 && ch <= 0xFF) )
		{
			outKishuIzon = true;
			return;
		}
	}
	//2�o�C�g�����`�F�b�N
	else if( text.GetItemCount() == 2 )
	{
		unsigned short ch = text.Get(0)*0x100 + text.Get(1);
		// ���̎��`���̑O
		if( ch >= 0x81AD && ch <= 0x81B7 )
		{
			outKishuIzon = true;
		}
		//���̎��`�Ȃ̑O
		else if( ch >= 0x81C0 && ch <= 0x81C7 )
		{
			outKishuIzon = true;
		}
		//�΂̎��`�ڂ̑O
		else if( ch >= 0x81CF && ch <= 0x81D9 )
		{
			outKishuIzon = true;
		}
		//��̎��`�̑O
		else if( ch >= 0x81E9 && ch <= 0x81EF )
		{
			outKishuIzon = true;
		}
		//���̎��`���̑O
		else if( ch >= 0x81F8 && ch <= 0x81FB )
		{
			outKishuIzon = true;
		}
		//���̎��`�O�̑O
		else if( ch >= 0x8201 && ch <= 0x8215 )
		{
			outKishuIzon = true;
		}
		//�X�̎��`�`�̑O
		else if( ch >= 0x8259 && ch <= 0x825F )
		{
			outKishuIzon = true;
		}
		//�y�̎��`���̑O
		else if( ch >= 0x827A && ch <= 0x8280 )
		{
			outKishuIzon = true;
		}
		//���̎��`���̑O
		else if( ch >= 0x829B && ch <= 0x829E )
		{
			outKishuIzon = true;
		}
		//��̎��`�@�̑O
		else if( ch >= 0x82F2 && ch <= 0x82FC )
		{
			outKishuIzon = true;
		}
		//���̎��`���̑O
		else if( ch >= 0x8397 && ch <= 0x839E )
		{
			outKishuIzon = true;
		}
		//���̎��`���̑O
		else if( ch >= 0x83B7 && ch <= 0x83BE )
		{
			outKishuIzon = true;
		}
		//�ւ̎��`�@�̑O
		else if( ch >= 0x83D7 && ch <= 0x83FC )
		{
			outKishuIzon = true;
		}
		//�`�̎��`�p�̑O
		else if( ch >= 0x8461 && ch <= 0x846F )
		{
			outKishuIzon = true;
		}
		//���̎��`���̑O
		else if( ch >= 0x8492 && ch <= 0x849E )
		{
			outKishuIzon = true;
		}
		//���̎��`���̑O
		else if( ch >= 0x84BF && ch <= 0x889E )
		{
			outKishuIzon = true;
		}
		//�r�̎��`���̑O
		else if( ch >= 0x9873 && ch <= 0x989E )
		{
			outKishuIzon = true;
		}
		//꤂̎��`
		else if( ch >= 0xEAA5 && ch <= 0xEFFC )
		{
			outKishuIzon = true;
		}
		//��ʃo�C�g�`�F�b�N
		else if( ( (ch >= 0x8100 && ch <= 0x9FFF) || (ch >= 0xE000 && ch <= 0xEFFF) ) == false )
		{
			outKishuIzon = true;
		}
		//���ʃo�C�g�`�F�b�N
		else if( ((ch&0xFF)<0x40) || ((ch&0xFF)==0x7F) || ((ch&0xFF)>0xFC ) )
		{
			outKishuIzon = true;
		}
	}
	else
	{
		_ACError("CheckTrueSJIS() error",NULL);
	}
}

/*
�Q�l�F
ATextEncodingWrapper::TECConvertFromUTF16()
Windows�Ŕg�_�b�V���̖�肪�������̂ŁA���L�̂悤�ȕW���}�b�s���O�{�Ǝ��}�b�s���O�n�C�u���b�h�\���̃R�[�h��g�񂾂��A
�g�_�b�V����Windows�̏ꍇ�ASJIS:0x8160<--->U+FF5E�Ƀ}�b�s���O�����̂ŁA
#200�̖��Ή���Windows�̏ꍇ�Ɏ��s���Ȃ��悤�ɂ��邾���Ŗ��΍�ł����B
���̐�ǂ����Ă��}�b�s���O�ɖ�肪�������ꍇ�̂��߂ɁA���L�R�[�h�̓R�����g�Ƃ��Ďc���B2010/5/1
	//�o�͐�S�폜
	outText.DeleteAll();
	
	ABool	result = true;
	try
	{
		//Convert��̈�m��
		outText.Reserve(0,inText.GetItemCount()*3 +1024);
		
		//SJIS��TextEncoding���擾
		//�Ȃ��AJIS/EUC�́ASJIS���o�R���ĕϊ�����̂ŁAinDst��JIS/EUC�ɂȂ邱�Ƃ͂Ȃ�
		ATextEncoding	shiftJISEncoding = ATextEncodingWrapper::GetSJISTextEncoding();
		
		//�epos�̓o�C�g�P�ʂňړ�
		AIndex	dstPos = 0;
		AIndex	srcPos = 0;
		AIndex	srcAnaPos = 0;
		{
			//�������|�C���^�擾
			AStTextPtr	intextptr(inText,0,inText.GetItemCount());
			AStTextPtr	outtextptr(outText,0,outText.GetItemCount());
			for( ; srcAnaPos < intextptr.GetByteCount(); srcAnaPos += sizeof(AUTF16Char) )
			{
				//srcAnaPos����Unicode�ꕶ���擾
				AUTF16Char	uniChar = inText.GetUTF16CharFromUTF16Text(srcAnaPos);
				ABool	replaceMode = false;
				AText	replaceChar;
				switch(uniChar)
				{
					//"?"
					//�g�_�b�V��U+301C
					//�S�p�`���_U+FF5E
					//Unicode�̎d�l���ɊԈႢ������AU+301C��?�̎��`���㉺�t�ɂȂ��Ă���B
					//�ǂݍ��ݎ��AU+FF5E�փ}�b�s���O�����̂́A���ꂪ�����Ǝv����B
				  case 0x301C:
				  case 0xFF5E:
					{
						if( inDst == shiftJISEncoding )
						{
							replaceChar.Add(0x81);
							replaceChar.Add(0x60);
							replaceMode = true;
						}
						break;
					}
				}
				//
				if( replaceMode == true )
				{
					//���݈ʒu�̕����͓Ǝ��ϊ��}�b�s���O�Œu��������
					
					//���݂̈ʒu�܂ł�OS�W���}�b�s���O�ŕϊ�
					int bytelen = ::WideCharToMultiByte(inDst,0,//�ϊ��I�v�V���������K�v
							reinterpret_cast<LPCWSTR>(intextptr.GetPtr()+srcPos),
							(srcAnaPos-srcPos)/sizeof(AUTF16Char),
							reinterpret_cast<LPSTR>(outtextptr.GetPtr()+dstPos),
							outtextptr.GetByteCount()-dstPos,
							NULL,NULL);
					//�������ݐ�|�C���^�ړ�
					dstPos += bytelen;
					//�Ǝ��ϊ�������������
					for( AIndex i = 0; i < replaceChar.GetItemCount(); i++ )
					{
						*(outtextptr.GetPtr()+dstPos) = replaceChar.Get(i);
						dstPos++;
					}
					//�ǂݍ��݌��|�C���^�ړ�
					srcPos = srcAnaPos + sizeof(AUTF16Char);
				}
			}
			//�Ō�Ɏc���OS�W���}�b�s���O�ŕϊ�
			int bytelen = ::WideCharToMultiByte(inDst,0,//�ϊ��I�v�V���������K�v
					reinterpret_cast<LPCWSTR>(intextptr.GetPtr()+srcPos),
					(intextptr.GetByteCount()-srcPos)/sizeof(AUTF16Char),
					reinterpret_cast<LPSTR>(outtextptr.GetPtr()+dstPos),
					outtextptr.GetByteCount()-dstPos,
					NULL,NULL);
			dstPos += bytelen;
		}
		//outText���g�p�������폜
		outText.Delete(dstPos,outText.GetItemCount()-dstPos);
	}
	catch(...)
	{
			outText.DeleteAll();
			result = false;
	}
	return result;
*/

