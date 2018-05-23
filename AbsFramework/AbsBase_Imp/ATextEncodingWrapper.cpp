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

MacOSX用テキストエンコーディング処理ラッパー


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
#pragma mark [クラス]ATextEncodingWrapper
#pragma mark ===========================

#pragma mark ---Init

/**
初期化
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
	//参考：http://msdn.microsoft.com/en-us/library/ms776446(VS.85).aspx 
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
	//50932だけ有効にするRegisterTextEncodingName("EUC-JP",20932);
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
TextEncoding名登録（Windows用）
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

#pragma mark ---TextEncoding変換

/**
指定TextEncoding->UTF8

ioTextのテキストをinTextEncodingからUTF-8に変換し、ioTextに格納する
@note 変換失敗の場合はfalseを返す。この場合、ioTextの内容は破壊されている。
*/
ABool	ATextEncodingWrapper::ConvertToUTF8( AText& ioText, const ATextEncoding inSrcTextEncoding, 
		const ABool inUseFallback, ABool& outFallbackUsed, const ABool inFromExternalData )//B0400
{
	if( ioText.GetItemCount() == 0 )   return true;
	
	ABool	result = true;
	outFallbackUsed = false;
	
	//下記のシーケンスによりinTextEncodingからUTF-8へ変換する
	//①BOM処理
	//②一部エンコーディングの場合、SJISへ変換
	//③UTF16へ変換
	//④UTF16→UTF8
	
	//処理途中現状での文字コードを格納
	ATextEncoding	curEncoding = inSrcTextEncoding;
	
	CFStringRef	strref = NULL;
	CFMutableStringRef	mstrref = NULL;
	try
	{
		//①BOM処理
		//UTF-8でBOMを含んでいたら、BOM削除する
		//B0000 2.1.7時代からBOMOffsetは未使用 AIndex	BOMoffset = 0;
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
		/*#1040 BOMの扱いはCFStringCreateWithBytes()に任せる（ただし、UTF-8の場合はCFStringCreateWithBytes()を実行しないので、上でBOM削除している）
		//UTF-16でBOMを含んでいたら、BOMに従って、使っているマシンのEndianに合わせたUTF-16に変換する
		if( curEncoding == GetUTF16TextEncoding() || curEncoding == GetUTF16LETextEncoding() || curEncoding == GetUTF16BETextEncoding() )
		{
			if( ioText.GetItemCount() >= 2 )
			{
				//BOMがFE FF
				if( ioText.Get(0) == 0xFE && ioText.Get(1) == 0xFF )
				{
					ioText.Delete(0,2);
					//IntelMacなら元データFE FF→FF FE（FEFFのLE配置）となるようにする
					if( UTF16IsLE() )
					{
						ChangeByteOrder(ioText);
					}
				}
				//BOMがFF FE
				else if( ioText.Get(0) == 0xFF && ioText.Get(1) == 0xFE )
				{
					ioText.Delete(0,2);
					//PowerPCMacなら元データFF FE→FE FF（FEFFのBE配置）となるようにする
					if( UTF16IsLE() == false )
					{
						ChangeByteOrder(ioText);
					}
				}
				//BOMがなくて、UTF16LE指定なら、元データがLE配置なので、PowerPCMacに限り、byte swap
				else if( curEncoding == GetUTF16LETextEncoding() )
				{
					if( UTF16IsLE() == false )
					{
						ChangeByteOrder(ioText);
					}
				}
				//BOMがなくて、UTF16BE指定なら、元データがBE配置なので、IntelMacに限り、byte swap B0377
				else if( curEncoding == GetUTF16BETextEncoding() )
				{
					if( UTF16IsLE() == true )
					{
						ChangeByteOrder(ioText);
					}
				}
				//BOMがなくて、UTF16指定なら、元データがBE配置なので、IntelMacに限り、byte swap B0400
				//ただし、外部（ファイル）からのデータの場合に限る（'utxt'やTECで変換したデフォルトUnicode16bitデータは、機種依存BE/LEとなるため。）
				else if( curEncoding == GetUTF16TextEncoding() && inFromExternalData == true )
				{
					if( UTF16IsLE() == true )
					{
						ChangeByteOrder(ioText);
					}
				}
				//以上により、使っているマシンのEndianに合わせたUTF-16に変換終了
				curEncoding = GetUTF16TextEncoding();
			}
		}
		*/
		
		if( curEncoding != GetUTF8TextEncoding() )
		{
			//変換元テキストエンコーディングのバイト列からCFStringを生成
			{
				AStTextPtr	intextptr(ioText,0,ioText.GetItemCount());
				strref = ::CFStringCreateWithBytes(kCFAllocatorDefault,intextptr.GetPtr(),intextptr.GetByteCount(),
												   curEncoding,inFromExternalData);
			}
			if( strref == NULL )
			{
				//CFStringCreateWithBytes()で変換エラーのときは、旧APIを使用して、フォールバック変換を試みる
				if( inUseFallback == true )
				{
					//JISの場合、JIS→SJISに変換
					if( curEncoding == GetJISTextEncoding() )
					{
						//B0126
						ConvertFromJISToSJIS(ioText);
						curEncoding = kCFStringEncodingDOSJapanese;
					}
					//EUCの場合、EUC→SJISに変換 B0141
					if( curEncoding == GetEUCTextEncoding() )
					{
						ConvertFromEUCToSJIS(ioText);
						curEncoding = kCFStringEncodingDOSJapanese;
					}
					//TextEncoding名を取得
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
					//旧APIのTextEncoding値を取得
					Str255	tecnamestr;
					tecname.GetPstring(tecnamestr,255);
					if( ::TECGetTextEncodingFromInternetName(&tec,tecnamestr) != noErr )   throw 0;
					//UTF16へ変換
					// 各種文字コード→UTF16
					{
						AText	buffer;
						if( TECConvertToUTF16(ioText,buffer,tec,inUseFallback,outFallbackUsed) == false )   throw 0;
						ioText.SwapContent(buffer);
					}
					
					//UTF16→UTF8
					AText	buffer;
					if( TECConvertUTFx(ioText,buffer,GetUTF16TextEncoding(),GetUTF8TextEncoding()) == false )   throw 0;
					ioText.SwapContent(buffer);
					return true;
				}
				//
				throw 0;
			}
			
			//CFMutableString生成
			mstrref = ::CFStringCreateMutableCopy(kCFAllocatorDefault,0,strref);
			if( mstrref == NULL )   throw 0;
			
			//JIS系共通の変換
			if( IsSJISSeriesTextEncoding(inSrcTextEncoding) == true || 
				inSrcTextEncoding == GetJISTextEncoding() || 
				inSrcTextEncoding == GetEUCTextEncoding() )
			{
				ConvertCFMutableStringChar(mstrref,0xFFE1,0x00A3);//半角￡へマッピング統一する
				ConvertCFMutableStringChar(mstrref,0xFFE0,0x00A2);//半角￠へマッピング統一する
				ConvertCFMutableStringChar(mstrref,0xFF0D,0x2212);
				ConvertCFMutableStringChar(mstrref,0x2225,0x2016);
				ConvertCFMutableStringChar(mstrref,0xFF5E,0x301C);
				ConvertCFMutableStringChar(mstrref,0xFFE2,0x00AC);
				ConvertCFMutableStringChar(mstrref,0x2015,0x2014);//SJIS:0x815CはU+2014（全角ダッシュ）へマッピング統一する
				ConvertCFMutableStringChar(mstrref,0x203E,0x007E);//SJIS:0x7EはU+007Eへマッピング統一する
				ConvertCFMutableStringChar(mstrref,0x00A5,0x005C);//#1106
			}
			
			//CFString全体のrangeを取得
			CFRange	range = ::CFRangeMake(0,::CFStringGetLength(mstrref));
			
			//現在のioTextを全削除
			ioText.DeleteAll();
			
			//変換後に必要なバイト長を取得
			// ioText.Reserve(0,::CFStringGetMaximumSizeForEncoding(::CFStringGetLength(mstrref),GetUTF8TextEncoding())+1024);
			CFIndex	resultLen = 0;
			::CFStringGetBytes(mstrref,range,GetUTF8TextEncoding(),0,false,NULL,0,&resultLen);
			
			//変換後文字列格納先確保
			ioText.Reserve(0,resultLen);
			
			//UTF-8文字列取得
			{
				AStTextPtr	textptr(ioText,0,ioText.GetItemCount());
				CFIndex	index = ::CFStringGetBytes(mstrref,range,GetUTF8TextEncoding(),0,false,
												   textptr.GetPtr(), textptr.GetByteCount(), &resultLen );
				//変換した文字数がCFStringの文字数と一致していなければエラーとする
				if( index != ::CFStringGetLength(mstrref) )   throw 0;
			}
			
			//ioText未使用部分を削除（多めに確保する方法は止めたので不要と思うが、念のため残す）
			ioText.Delete(resultLen,ioText.GetItemCount()-resultLen);
			
			/*#1040
			//B0126 TECSetBasicOptionsがMacOSX10.2で使用できないので、TECConvertTextは使用不可
			//よって、ConvertFromTextToUnicodeでUTF16に変換、その後、UTF8に変換する
			
			//②一部エンコーディングの場合、SJISへ変換
			//JISの場合、JIS→SJISに変換
			if( curEncoding == GetJISTextEncoding() )
			{
				//B0126
				ConvertFromJISToSJIS(ioText);
				curEncoding = GetSJISTextEncoding();
			}
			//EUCの場合、EUC→SJISに変換 B0141
			if( curEncoding == GetEUCTextEncoding() )
			{
				ConvertFromEUCToSJIS(ioText);
				curEncoding = GetSJISTextEncoding();
			}
			
			//③UTF16へ変換
			// 各種文字コード→UTF16
			if( curEncoding != GetUTF16TextEncoding() )
			{
				AText	buffer;
				if( TECConvertToUTF16(ioText,buffer,curEncoding,inUseFallback,outFallbackUsed) == false )   throw 0;
				ioText.SwapContent(buffer);
			}
			
			//#369 携帯絵文字
			*
			この位置で対応予定
			*
			
			//④UTF16→UTF8
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
	
	//CFString解放
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
UTF-8->指定TextEncoding

ioTextのテキストをUTF-8からinTextEncodingに変換し、ioTextに格納する
変換失敗の場合はfalseを返す。この場合、ioTextの内容は破壊されている。
*/
ABool	ATextEncodingWrapper::ConvertFromUTF8( AText& ioText, const ATextEncoding inDstTextEncoding, 
											   const ABool inConvertToCanonical )//#1044
{
	if( ioText.GetItemCount() == 0 )   return true;
	
	// 1. UTF-8の場合、これで終了
	if( inDstTextEncoding == GetUTF8TextEncoding() )
	{
		//NFC形式への自動変換（UTF-8保存の場合でもこれだけは処理する） #1044
		if( inConvertToCanonical == true )
		{
			ConvertToCanonicalComp(ioText);
		}
		return true;
	}
	
	//#1040
	//現在のUTF-8テキストからCFString生成
	AStCreateCFStringFromAText	str(ioText);
	
	ABool	result = true;
	CFMutableStringRef	mstrref = NULL;
	try 
	{
		//CFMutableString生成
		mstrref = ::CFStringCreateMutableCopy(kCFAllocatorDefault,0,str.GetRef());
		if( mstrref == NULL )   throw 0;
		
		//JIS系共通の変換
		if( IsSJISSeriesTextEncoding(inDstTextEncoding) == true || 
			inDstTextEncoding == GetJISTextEncoding() || 
			inDstTextEncoding == GetEUCTextEncoding() )
		{
			//￠の扱い
			//→U+FFE0だとSJIS(Mac), SJIS(0208)で変換できないのでU+00A2に変換する。
			//（SJIS(Win)ではU+00A2でも変換OK）
			ConvertCFMutableStringChar(mstrref,0xFFE0,0x00A2);
			//￡の扱い
			//→U+FFE1だとSJIS(Mac), SJIS(0208)で変換できないのでU+00A3に変換する。
			//（SJIS(Win)ではU+00A3でも変換OK）
			ConvertCFMutableStringChar(mstrref,0xFFE1,0x00A3);
			//－（全角ハイフンマイナス）の扱い
			//→U+FF0DだとSJIS(Mac), SJIS(0208)で変換できないのでU+2212に変換する。
			ConvertCFMutableStringChar(mstrref,0xFF0D,0x2212);
			//∥（平行記号）の扱い
			//→U+2225だとSJIS(Mac), SJIS(0208)で変換できないのでU+2016に変換する。
			ConvertCFMutableStringChar(mstrref,0x2225,0x2016);
			//～（波ダッシュ）の扱い
			ConvertCFMutableStringChar(mstrref,0xFF5E,0x301C);
			//￢（否定記号）の扱い
			ConvertCFMutableStringChar(mstrref,0xFFE2,0x00AC);
			//ダッシュ(SJIS:0x815C)の扱い
			//JIS規格、Mac、Vista以降：U+2014
			//Unicodeコンソーシアム、CP932：U+2015
			//→U+2015だとSJIS(Mac), SJIS(0208)で変換できないのでU+2014に変換する。
			//（SJIS(Win)ではU+2014でも変換OK）
			ConvertCFMutableStringChar(mstrref,0x2015,0x2014);
		}
		//半角￥、バックスラッシュの処理
		switch(inDstTextEncoding)
		{
			//半角￥(U+00A5)へ置換
		  case kCFStringEncodingMacJapanese:
		  case kCFStringEncodingShiftJIS:
		  case kCFStringEncodingShiftJIS_X0213:
		  case kCFStringEncodingShiftJIS_X0213_MenKuTen:
			{
				//SJIS(Mac), SJIS(0208)ではU+00A5→0x5C、U+005C→0x80にマッピングされる
				//よって、U+005CはU+00A5へ置換する
				ConvertCFMutableStringChar(mstrref,0x005C,0x00A5);
				break;
			}
			//バックスラッシュ(U+005C)へ置換
		  case kCFStringEncodingDOSJapanese:
		  case kCFStringEncodingEUC_JP:
		  case kCFStringEncodingISO_2022_JP:
		  case kCFStringEncodingISO_2022_JP_1:
		  case kCFStringEncodingISO_2022_JP_2:
		  case kCFStringEncodingISO_2022_JP_3:
			{
				//SJIS(Win)ではU+00A5→変換エラー、U+005C→0x5Cにマッピングされる
				//よって、U+00A5はU+005Cへ置換する
				ConvertCFMutableStringChar(mstrref,0x00A5,0x005C);
				break;
			}
		}
		//半角チルダ、オーバーラインの処理
		switch(inDstTextEncoding)
		{
			//オーバーライン(U+203E)へ置換
		  case kCFStringEncodingShiftJIS:
		  case kCFStringEncodingShiftJIS_X0213:
		  case kCFStringEncodingShiftJIS_X0213_MenKuTen:
			{
				//SJIS(0208)ではU+007E→変換エラー、U+203E→0x7Eにマッピングされる
				//よって、U+007EはU+203Eへ置換する
				ConvertCFMutableStringChar(mstrref,0x007E,0x203E);
				break;
			}
			//半角チルダ(U+007E)へ置換
		  case kCFStringEncodingMacJapanese:
		  case kCFStringEncodingDOSJapanese:
		  case kCFStringEncodingEUC_JP:
		  case kCFStringEncodingISO_2022_JP:
		  case kCFStringEncodingISO_2022_JP_1:
		  case kCFStringEncodingISO_2022_JP_2:
		  case kCFStringEncodingISO_2022_JP_3:
			{
				//SJIS(Mac), SJIS(Win)ではU+007E→0x7E、U+203E→変換エラーにマッピングされる
				//よって、U+203EはU+007Eへ置換する
				ConvertCFMutableStringChar(mstrref,0x203E,0x007E);
				break;
			}
		}
		
		//Canonical Comp形式への変換
		if( inConvertToCanonical == true )
		{
			//Normalize実行
			::CFStringNormalize(mstrref,kCFStringNormalizationFormC);
		}
		
		//CFString全体のrangeを取得
		CFRange	range = ::CFRangeMake(0,::CFStringGetLength(mstrref));
		
		//現在のioTextを全削除
		ioText.DeleteAll();
		
		//変換後に必要なバイト長を取得
		//ioText.Reserve(0,::CFStringGetMaximumSizeForEncoding(::CFStringGetLength(str.GetRef())+1024,inDstTextEncoding));
		CFIndex	resultLen = 0;
		::CFStringGetBytes(mstrref,range,inDstTextEncoding,0,false,NULL,0,&resultLen);
		
		//変換後文字列格納先確保
		ioText.Reserve(0,resultLen);
		
		//指定テキストエンコーディングのバイト列取得（BOMはここでは付けない）
		{
			AStTextPtr	textptr(ioText,0,ioText.GetItemCount());
			CFIndex	index = ::CFStringGetBytes(mstrref,range,inDstTextEncoding,0,false,
											   textptr.GetPtr(), textptr.GetByteCount(), &resultLen );
			//変換した文字数がCFStringの文字数と一致していなければエラーとする
			if( index != ::CFStringGetLength(mstrref) )   throw 0;
		}
		//ioText未使用部分を削除（多めに確保する方法は止めたので不要と思うが、念のため残す）
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
	//UTF-8以外の場合、いったんUTF-16に変換してioTextに格納
	//B0000 ABool	result = true;
	{
		//いったんUTF-16へ変換
		//（inConvertToCanonicalがtrueのときはUTF-16のCanonical Compositionに変換する）
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
	
	//JIS系の場合、0x00A5は0x005Cへ変換する（一応設定save5CForYenWhenJISは用意するがとりあえず常に変換にする）
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
			//Mac OS X用変換補正
			
			//Macの場合はU+FF5E(全角チルダ)があるとJIS系で保存できない　#200
			switch(*ptr_uc)
			{
			  case 0xFF0D:
				{
					*ptr_uc = 0x2212;//－
					break;
				}
			  case 0xFF5E:
				{
					*ptr_uc = 0x301C;//～
					break;
				}
			}
#endif
#if IMPLEMENTATION_FOR_WINDOWS
			//Windows用変換補正
			
			//Windowsの場合はU+301C(波ダッシュ)をJIS系で保存できない（"?"に化ける）
			switch(*ptr_uc)
			{
			  case 0x301C:
				{
					*ptr_uc = 0xFF5E;//～
					break;
				}
			}
#endif
		}
	}
	
	// 2. UTF-16の場合、これで終了
	if( inDstTextEncoding == GetUTF16TextEncoding() )
	{
		return true;
	}
	// 3. UTF-16LEの場合、PowerPCMacなら、byte orderを変換して終了
	else if( inDstTextEncoding == GetUTF16LETextEncoding() )
	{
		if( UTF16IsLE() == false )
		{
			ChangeByteOrder(ioText);
		}
		return true;
	}
	//B0377 UTF-16BEの場合、IntelMacなら、byte orderを変換して終了
	else if( inDstTextEncoding == GetUTF16BETextEncoding() )
	{
		if( UTF16IsLE() == true )
		{
			ChangeByteOrder(ioText);
		}
		return true;
	}
	// 4. JISの場合
	else if( inDstTextEncoding == GetJISTextEncoding() )
	{
		//UTF16->SJIS
		AText	buffer;
		if( TECConvertFromUTF16(ioText,buffer,GetSJISTextEncoding()) == false )
		{
			//変換エラー
			return false;
		}
		//SJIS->JIS
		//B0126
		{
			ConvertFromSJISToJIS(buffer,ioText);
		}
		return true;
	}
	// 5. EUCの場合 B0141
	else if( inDstTextEncoding == GetEUCTextEncoding() )
	{
		//UTF16->SJIS
		AText	buffer;
		if( TECConvertFromUTF16(ioText,buffer,GetSJISTextEncoding()) == false )
		{
			//変換エラー
			return false;
		}
		//SJIS->EUC
		{
			ConvertFromSJISToEUC(buffer,ioText);
		}
		return true;
	}
	// それ以外
	else
	{
		//UTF16->それぞれのTextEncoding
		AText	buffer;
		if( TECConvertFromUTF16(ioText,buffer,inDstTextEncoding) == false )
		{
			//変換エラー
			return false;
		}
		ioText.SwapContent(buffer);
		return true;
	}
	*/
}

//#1040
/**
CFMutableString文字置換
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
SJIS系テキストエンコーディングかどうかを判定
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
UTF-8をUTF-8(CanonicalComp)形式へ変換
CanonicalComp:ひらがな＋濁点、アルファベット＋ウムラウト等を一文字で表す標準形(NFC形式)
*/
void	ATextEncodingWrapper::ConvertToCanonicalComp( AText& ioText )
{
#if IMPLEMENTATION_FOR_MACOSX
	/*#1040
	AText	buffer;
	TECConvertFromUTF16(ioUTF16Text,buffer,::CreateTextEncoding(kTextEncodingUnicodeDefault,kUnicodeCanonicalCompVariant,kUnicode16BitFormat));
	ioUTF16Text.SetText(buffer);
	*/
	//CFMutableString生成
	AStCreateCFStringFromAText	str(ioText);
	CFMutableStringRef	mstrref = ::CFStringCreateMutableCopy(kCFAllocatorDefault,0,str.GetRef());
	if( mstrref != NULL )
	{
		//Normalize実行
		::CFStringNormalize(mstrref,kCFStringNormalizationFormC);
		//結果設定
		ioText.SetFromCFString(mstrref);
		//CFMutableString解放
		::CFRelease(mstrref);
	}

#elif IMPLEMENTATION_FOR_WINDOWS
	/*Normaliz.dllが必要→AUnicodeData::NormalizeD()を使う
	AStTextPtr	intextptr(ioUTF16Text,0,ioUTF16Text.GetItemCount());
	AText	buffer;
	buffer.Reserve(0,ioUTF16Text.GetItemCount()+1024);//NFCへの変換なら元のテキストサイズから増えることはないだろうので
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
無印UTF16がLittleEndianかどうかを返す
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
	//Windowsの場合は常にLEで扱う
	return true;
#endif
}

#if IMPLEMENTATION_FOR_MACOSX
/**
UTF-16をHFSPlusDecomp形式へ変換
*/
void	ATextEncodingWrapper::ConvertUTF16ToHFSPlusDecomp( AText& ioUTF16Text )
{
	AText	buffer;
	TECConvertFromUTF16(ioUTF16Text,buffer,::CreateTextEncoding(kTextEncodingUnicodeDefault,kUnicodeHFSPlusDecompVariant,kUnicode16BitFormat));
	ioUTF16Text.SetText(buffer);
}
#endif

#pragma mark ===========================

#pragma mark ---TextEncoding独自変換モジュール部分

/**
ByteOrder入れ替え
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

//B0126 TECConvertTextを使うと\が全角に化けたりするので独自ルーチンを使用する。
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
			//半角カナモード #325 
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
						//半角カナモードへ移行#325
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
				if( ch == 0x8E )//半角カナなど
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

//B0126 TECConvertTextを使うと\が全角に化けたりするので独自ルーチンを使用する。
/**
SJIS-JIS
*/
void	ATextEncodingWrapper::ConvertFromSJISToJIS( const AText& inText, AText& outBuffer )
{
	//出力先全削除
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
		//半角カナ #325
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
		//ASCII文字→エスケープ解除
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
		//漢字
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
	//出力先全削除
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
			if( ch1 >= 0x80 )//半角カナなど
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
SJISでの漢字文字かどうかを取得
*/
ABool	ATextEncodingWrapper::IsSJISKanjiChar( const AUChar c )
{
	return ((c>=0x81&&c<=0x9F)||(c>=0xE0&&c<=0xFC));
}

#pragma mark ===========================

#pragma mark ---TextEncoding変換用内部モジュール

// 各種エンコーディング->UTF16
//B0126
/**
指定TextEncoding->UTF-16
*/
ABool	ATextEncodingWrapper::TECConvertToUTF16( const AText& inText, AText& outText, 
		const ATextEncoding inSrc, ABool inUseFallback, ABool &outFallbackUsed )
{
#if IMPLEMENTATION_FOR_MACOSX
	//出力先全削除
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
		//Convert先領域確保
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
		
		//outText未使用部分を削除
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
	//出力先全削除
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
			//Convert先サイズ計算・メモリ確保
			unilen = ::MultiByteToWideChar(inSrc,MB_ERR_INVALID_CHARS,//MB_ERR_INVALID_CHARSがない場合、エラー文字があっても無理やり変換される
					reinterpret_cast<LPCSTR>(intextptr.GetPtr()),intextptr.GetByteCount(),
					NULL,0);
			outText.Reserve(0,unilen*sizeof(wchar_t)+1024);
			
			AStTextPtr	outtextptr(outText,0,outText.GetItemCount());
			//Convert実行
			unilen = ::MultiByteToWideChar(inSrc,MB_ERR_INVALID_CHARS,//MB_ERR_INVALID_CHARSがない場合、エラー文字があっても無理やり変換される
					reinterpret_cast<LPCSTR>(intextptr.GetPtr()),intextptr.GetByteCount(),
					reinterpret_cast<LPWSTR>(outtextptr.GetPtr()),outtextptr.GetByteCount()/sizeof(AUTF16Char));
		}
		DWORD	err = GetLastError();
		if( unilen == 0 )
		{
			//_ACThrow("MultiByteToWideChar error",NULL);
			//CP番号未対応など。52932等もエラーとなる。
			return false;
		}
		
		AByteCount	resultLen = unilen*sizeof(wchar_t);
		//outText未使用部分を削除
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

// UTF16->各種エンコーディング
// なぜかUTF16->JISができない
//検討必要★　こちらにfallbackオプションは必要ないのか（こちらこそ必要ではないのか）
/**
UTF-16->指定TextEncoding
*/
ABool	ATextEncodingWrapper::TECConvertFromUTF16( const AText& inText, AText& outText, const TextEncoding inDst )
{
#if IMPLEMENTATION_FOR_MACOSX
	//出力先全削除
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
		//Convert先領域確保
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
		
		//outText未使用部分を削除
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
	//出力先全削除
	outText.DeleteAll();
	
	ABool	result = true;
	try
	{
		int	bytelen = 0;
		{
			AStTextPtr	intextptr(inText,0,inText.GetItemCount());
			//Convert先サイズ計算・メモリ確保
			bytelen = ::WideCharToMultiByte(inDst,0,
					reinterpret_cast<LPCWSTR>(intextptr.GetPtr()),intextptr.GetByteCount()/sizeof(AUTF16Char),
					NULL,0,
					NULL,NULL);//default charはデフォルト
			outText.Reserve(0,bytelen+1024);
			
			AStTextPtr	outtextptr(outText,0,outText.GetItemCount());
			//Convert実行
			bytelen = ::WideCharToMultiByte(inDst,0,
					reinterpret_cast<LPCWSTR>(intextptr.GetPtr()),intextptr.GetByteCount()/sizeof(AUTF16Char),
					reinterpret_cast<LPSTR>(outtextptr.GetPtr()),outtextptr.GetByteCount(),
					NULL,NULL);
		}
		
		//outText未使用部分を削除
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

// TECConvertTextでは変換不能文字が勝手に?に変換される
// ~や\が勝手に別の文字に変換されたりするので、UTF8<->UTF16のみに使用する
//変換エラーの場合はfalseを返し、ioTextは変更されない
/**
UTF8<->UTF16変換
*/
ABool	ATextEncodingWrapper::TECConvertUTFx( const AText& inText, AText& outText, const ATextEncoding inSrc, const ATextEncoding inDst )
{
#if IMPLEMENTATION_FOR_MACOSX
	//出力先全削除
	outText.DeleteAll();
	
	TECObjectRef	converter = NULL;//B0136
	ABool	converterCreated = false;//R0108
	ABool	result = true;
	try
	{
		//Convert先領域確保
		outText.Reserve(0,inText.GetItemCount()*3 +1024);
		
		OSStatus	status;
		//R0108 可能な限りmTextEncodingPrefであらかじめ作成したconverterを使用する（高速化）
		//（メインスレッドの場合のみ。converterをスレッド間で共有できない。
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
		
		//outText未使用部分を削除
		outText.Delete(resultLen,outText.GetItemCount()-resultLen);
	}
	catch(...)//B0136
	{
		result = false;
	}
	//フラッシュ
	unsigned char	buffer[4096];
	ByteCount	actual = 0;
	::TECFlushText(converter,buffer,4095,&actual);
	if( converterCreated && (converter != NULL) )   ::TECDisposeConverter(converter);//B0136 converter解放漏れ修正
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
JIS文字->SJIS文字
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
EUC文字->JIS文字
*/
void	ATextEncodingWrapper::euc2sjis( unsigned char &a, unsigned char &b )
{
	a &= 0x7f;
	b &= 0x7f;
	
	jis2sjis( a, b );
}

/**
SJIS文字->JIS文字
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
SJIS文字->EUC文字
*/
void	ATextEncodingWrapper::sjis2euc( unsigned char &a, unsigned char &b )
{
	sjis2jis( a, b );
	a |= 0x80;
	b |= 0x80;
}

#pragma mark ===========================

#pragma mark ---TextEncoding取得

/**
IANA名称からATextEncodingを取得
@note 見つからない場合はUTF8を返す
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
IANA名称からATextEncodingを取得
*/
ABool	ATextEncodingWrapper::GetTextEncodingFromName( AConstCharPtr inName, ATextEncoding& outEncoding )
{
	AText	text(inName);
	return GetTextEncodingFromName(text,outEncoding);
}

/**
mi設定名称からATextEncodingを取得
*/
ABool	ATextEncodingWrapper::GetTextEncodingFromName( const AText& inName, ATextEncoding& outEncoding )
{
	//--------------------Shift_JIS用特殊処理-------------------- 
	//従来のTEC ManagerでのShift_JISでの変換はCP932ベースだったことから、
	//2.1, 3.0.0b5以前でShift_JISで保存するとCP932ベースで保存されている。
	//そのため、下記のようにする。
	//DocPrefに新設定kTextEncodingNameCocoaを追加する。
	//1. 旧設定"Shift_JIS"・新設定""の場合：ここには"Shift_JIS"で来る→kCFStringEncodingDOSJapanese(=Windows-31J)として扱う
	// 保存時は、旧設定"Shift_JIS"、新設定"Shift_JIS"
	//2. 旧設定"Shift_JIS"・新設定"Shift_JIS"の場合：ここには"Shift_JIS"で来る→kCFStringEncodingDOSJapanese(=Windows-31J)として扱う
	// 保存時は、旧設定"Shift_JIS"、新設定"Shift_JIS"
	//3. 旧設定"Shift_JIS"・新設定"Shift_JIS_X_0208"の場合：ここには"Shift_JIS_X_0208"で来る→kCFStringEncodingShiftJIS(=標準JIS)として扱う
	// 保存時は、旧設定"Shift_JIS"、新設定"Shift_JIS_X_0208"
	if( inName.CompareIgnoreCase("Shift_JIS") == true )
	{
		outEncoding = kCFStringEncodingDOSJapanese;//Windows-31Jに対応
		return true;
	}
	if( inName.CompareIgnoreCase("Shift_JIS_X_0208") == true )
	{
		outEncoding = kCFStringEncodingShiftJIS;
		return true;
	}
	
	//IANA名称からATextEncodingを取得
	return GetTextEncodingFromIANAName(inName,outEncoding);
}

//#1040
/**
IANA名称からATextEncodingを取得
*/
ABool	ATextEncodingWrapper::GetTextEncodingFromIANAName( const AText& inName, ATextEncoding& outEncoding )
{
#if IMPLEMENTATION_FOR_MACOSX
	//Shift_JISX0213はOS10.9でもAPIでは取得できなかったので、従来処理と同様に、下記3つは自分で設定。
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
	
	//--------------------IANA名から取得--------------------
	AStCreateCFStringFromAText	name(inName);
    outEncoding = ::CFStringConvertIANACharSetNameToEncoding(name.GetRef());
	if( outEncoding != kCFStringEncodingInvalidId )
	{
		return true;
	}
	else
	{
		//#372 <>内の数値で指定
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
	//OSバージョンによってはいくつかのEncodingがTECGetTextEncodingFromInternetNameでは取得できないのでCreateTextEncodingで取得する。
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
		//#372 <>内の数値で指定
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
	outEncoding = 932;//とりあえずSJIS
	AIndex	index = sTextEncodingArray_Name.Find(inName);
	if( index == kIndex_Invalid )
	{
		//<>内をCP番号とみなす
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
ATextEncodingからmi設定用名称を取得
*/
ABool	ATextEncodingWrapper::GetTextEncodingName( const ATextEncoding& inEncoding, AText& outName )
{
	outName.DeleteAll();
	//Shift_JIS用特殊処理
	//ATextEncoding：kCFStringEncodingDOSJapaneseは、設定での名称"Shift_JIS"に対応
	//ATextEncoding：kCFStringEncodingShiftJISは、設定での名称"Shift_JIS_X_0208"に対応
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
	//ATextEncodingからそのIANA名称を取得
	return GetTextEncodingIANAName(inEncoding,outName);
}

//#1040
/**
ATextEncodingからそのIANA名称を取得
*/
ABool	ATextEncodingWrapper::GetTextEncodingIANAName( const ATextEncoding& inEncoding, AText& outName )
{
#if IMPLEMENTATION_FOR_MACOSX
	//OSバージョンによってはTECGetTextEncodingInternetNameで取得できなかったらしいので今回も直接設定。
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
	//OSのAPIによりIANA名取得
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
	//OSバージョンによってはTECGetTextEncodingInternetNameで取得できないので直接設定。
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
		//★WindowsにはCPから名前取得するAPIはなさそう
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
指定名称のTextEncodingが現在動作中のOSで使用可能かどうかをチェックする
*/
ABool	ATextEncodingWrapper::CheckTextEncodingAvailability( const AText& inName )
{
#if IMPLEMENTATION_FOR_MACOSX
	ATextEncoding	enc;
	if( GetTextEncodingFromName(inName,enc) == true )
	{
		//半角スペースをUTF8から変換→UTF8へ変換できるかどうかを試す。
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
		// JIS, SJIS, UTF8, UTF16, UTF16LEはConvertFromTextToUnicodeを使わないので無条件OK
		if( enc == GetJISTextEncoding() || enc == GetUTF8TextEncoding() ||
				enc == GetEUCTextEncoding() || enc == GetUTF16LETextEncoding() || 
			enc == GetUTF16TextEncoding() || enc == GetUTF16BETextEncoding() )
		{
			return true;
		}
		// UTF-16→enc, enc→UTF-16の変換が成功するか試す
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
		// JIS, SJIS, EUC, UTF-8, UTF-16は無条件OK
		if( enc == GetSJISTextEncoding() || enc == GetJISTextEncoding() || enc == GetEUCTextEncoding() ||
					enc == GetUTF8TextEncoding() ||
					enc == GetUTF16TextEncoding() || 
					enc == GetUTF16LETextEncoding() || enc == GetUTF16BETextEncoding() )
		{
			return true;
		}
		// UTF-16→enc, enc→UTF-16の変換が成功するか試す
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
Converter取得
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
テキストエンコーディング設定名称をCarbon版用設定名称へ変換
*/
void ATextEncodingWrapper::ConvertToCarbonTextEncodingName( AText& ioText ) 
{
	if( ioText.Compare("Shift_JIS_X_0208") == true )
	{
		ioText.SetCstring("Shift_JIS");
	}
}

/**
JIS TextEncoding取得
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
EUC TextEncoding取得
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
SJIS TextEncoding取得
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
SJIS-X-0213 TextEncoding取得
*/
ATextEncoding ATextEncodingWrapper::GetSJISX0213TextEncoding()
{
#if IMPLEMENTATION_FOR_MACOSX
	return sSJISX0213TextEncoding;
#elif IMPLEMENTATION_FOR_WINDOWS
	//★WindowsはSJIS 0213未対応と思われる。932は絵文字（外字？）エンコーディングになる
//	_ACError("not implemented",NULL);
	return 932;
#else
	not implemented
#endif
}

/**
UTF-8 TextEncoding取得
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
UTF-16 TextEncoding取得
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
UTF-16(Canonical Composition) TextEncoding取得
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
UTF-16LE TextEncoding取得
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
UTF-16BE TextEncoding取得
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
OSデフォルトのTextEncoding取得
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
フォント名からLegacyテキストエンコーディングを取得
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
	//★暫定
	return 932;
#else
	not implemented
#endif
}

//#189
/**
各言語のLegacyテキストエンコーディングを取得
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
	//★暫定
	return 932;
#else
	not implemented
#endif
}
#endif

//#208
/**
JIS X 208で定義された文字かどうかをチェックする
*/
void	ATextEncodingWrapper::CheckJISX0208( const ATextEncoding inTextEncoding, const AText& inText, ABool& outKishuIzon, ABool& outHankakuKana )
{
	//参考：http://charset.7jp.net/
	//参考：http://www.asahi-net.or.jp/~ax2s-kmtn/index.html
	
	//返り値初期化
	outKishuIzon = false;
	outHankakuKana = false;
	//SJISへ変換
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
		//処理無し
	}
	else return;//JIS, SJIS, EUC以外はチェックしない
	//1バイト文字チェック
	if( text.GetItemCount() == 1 )
	{
		AUChar	ch = text.Get(0);
		if( ch >= 0xA1 && ch <= 0xDF )
		{
			//半角カナ
			outHankakuKana = true;
			return;
		}
		else if( (ch >= 0x80 && ch <= 0xA0) || (ch >= 0xE0 && ch <= 0xFF) )
		{
			outKishuIzon = true;
			return;
		}
	}
	//2バイト文字チェック
	else if( text.GetItemCount() == 2 )
	{
		unsigned short ch = text.Get(0)*0x100 + text.Get(1);
		// 〓の次～∈の前
		if( ch >= 0x81AD && ch <= 0x81B7 )
		{
			outKishuIzon = true;
		}
		//∩の次～∧の前
		else if( ch >= 0x81C0 && ch <= 0x81C7 )
		{
			outKishuIzon = true;
		}
		//∃の次～∠の前
		else if( ch >= 0x81CF && ch <= 0x81D9 )
		{
			outKishuIzon = true;
		}
		//∬の次～の前
		else if( ch >= 0x81E9 && ch <= 0x81EF )
		{
			outKishuIzon = true;
		}
		//¶の次～◯の前
		else if( ch >= 0x81F8 && ch <= 0x81FB )
		{
			outKishuIzon = true;
		}
		//◯の次～０の前
		else if( ch >= 0x8201 && ch <= 0x8215 )
		{
			outKishuIzon = true;
		}
		//９の次～Ａの前
		else if( ch >= 0x8259 && ch <= 0x825F )
		{
			outKishuIzon = true;
		}
		//Ｚの次～ａの前
		else if( ch >= 0x827A && ch <= 0x8280 )
		{
			outKishuIzon = true;
		}
		//ｚの次～ぁの前
		else if( ch >= 0x829B && ch <= 0x829E )
		{
			outKishuIzon = true;
		}
		//んの次～ァの前
		else if( ch >= 0x82F2 && ch <= 0x82FC )
		{
			outKishuIzon = true;
		}
		//ヶの次～Αの前
		else if( ch >= 0x8397 && ch <= 0x839E )
		{
			outKishuIzon = true;
		}
		//Ωの次～αの前
		else if( ch >= 0x83B7 && ch <= 0x83BE )
		{
			outKishuIzon = true;
		}
		//ωの次～Аの前
		else if( ch >= 0x83D7 && ch <= 0x83FC )
		{
			outKishuIzon = true;
		}
		//Яの次～аの前
		else if( ch >= 0x8461 && ch <= 0x846F )
		{
			outKishuIzon = true;
		}
		//яの次～─の前
		else if( ch >= 0x8492 && ch <= 0x849E )
		{
			outKishuIzon = true;
		}
		//╂の次～亜の前
		else if( ch >= 0x84BF && ch <= 0x889E )
		{
			outKishuIzon = true;
		}
		//腕の次～弌の前
		else if( ch >= 0x9873 && ch <= 0x989E )
		{
			outKishuIzon = true;
		}
		//熙の次～
		else if( ch >= 0xEAA5 && ch <= 0xEFFC )
		{
			outKishuIzon = true;
		}
		//上位バイトチェック
		else if( ( (ch >= 0x8100 && ch <= 0x9FFF) || (ch >= 0xE000 && ch <= 0xEFFF) ) == false )
		{
			outKishuIzon = true;
		}
		//下位バイトチェック
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
参考：
ATextEncodingWrapper::TECConvertFromUTF16()
Windowsで波ダッシュの問題があったので、下記のような標準マッピング＋独自マッピングハイブリッド構成のコードを組んだが、
波ダッシュはWindowsの場合、SJIS:0x8160<--->U+FF5Eにマッピングされるので、
#200の問題対応をWindowsの場合に実行しないようにするだけで問題対策できた。
この先どうしてもマッピングに問題が生じた場合のために、下記コードはコメントとして残す。2010/5/1
	//出力先全削除
	outText.DeleteAll();
	
	ABool	result = true;
	try
	{
		//Convert先領域確保
		outText.Reserve(0,inText.GetItemCount()*3 +1024);
		
		//SJISのTextEncodingを取得
		//なお、JIS/EUCは、SJISを経由して変換するので、inDstがJIS/EUCになることはない
		ATextEncoding	shiftJISEncoding = ATextEncodingWrapper::GetSJISTextEncoding();
		
		//各posはバイト単位で移動
		AIndex	dstPos = 0;
		AIndex	srcPos = 0;
		AIndex	srcAnaPos = 0;
		{
			//メモリポインタ取得
			AStTextPtr	intextptr(inText,0,inText.GetItemCount());
			AStTextPtr	outtextptr(outText,0,outText.GetItemCount());
			for( ; srcAnaPos < intextptr.GetByteCount(); srcAnaPos += sizeof(AUTF16Char) )
			{
				//srcAnaPosからUnicode一文字取得
				AUTF16Char	uniChar = inText.GetUTF16CharFromUTF16Text(srcAnaPos);
				ABool	replaceMode = false;
				AText	replaceChar;
				switch(uniChar)
				{
					//"?"
					//波ダッシュU+301C
					//全角チルダU+FF5E
					//Unicodeの仕様書に間違いがあり、U+301Cは?の字形が上下逆になっている。
					//読み込み時、U+FF5Eへマッピングされるのは、これが原因と思われる。
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
					//現在位置の文字は独自変換マッピングで置き換える
					
					//現在の位置までをOS標準マッピングで変換
					int bytelen = ::WideCharToMultiByte(inDst,0,//変換オプション検討必要
							reinterpret_cast<LPCWSTR>(intextptr.GetPtr()+srcPos),
							(srcAnaPos-srcPos)/sizeof(AUTF16Char),
							reinterpret_cast<LPSTR>(outtextptr.GetPtr()+dstPos),
							outtextptr.GetByteCount()-dstPos,
							NULL,NULL);
					//書き込み先ポインタ移動
					dstPos += bytelen;
					//独自変換文字書き込み
					for( AIndex i = 0; i < replaceChar.GetItemCount(); i++ )
					{
						*(outtextptr.GetPtr()+dstPos) = replaceChar.Get(i);
						dstPos++;
					}
					//読み込み元ポインタ移動
					srcPos = srcAnaPos + sizeof(AUTF16Char);
				}
			}
			//最後に残りをOS標準マッピングで変換
			int bytelen = ::WideCharToMultiByte(inDst,0,//変換オプション検討必要
					reinterpret_cast<LPCWSTR>(intextptr.GetPtr()+srcPos),
					(intextptr.GetByteCount()-srcPos)/sizeof(AUTF16Char),
					reinterpret_cast<LPSTR>(outtextptr.GetPtr()+dstPos),
					outtextptr.GetByteCount()-dstPos,
					NULL,NULL);
			dstPos += bytelen;
		}
		//outText未使用部分を削除
		outText.Delete(dstPos,outText.GetItemCount()-dstPos);
	}
	catch(...)
	{
			outText.DeleteAll();
			result = false;
	}
	return result;
*/

