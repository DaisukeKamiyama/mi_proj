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

AText.h

*/

#pragma once

#include "AArray.h"
#include "AStackOnlyObject.h"

class AUniText;
class ATextArray;

#pragma mark ===========================
#pragma mark [クラス]AText

//ATextの文字コードはUTF-8であることを前提とする
/**
Textクラス
*/
class AText : public AArray<AUChar>
{
	friend class AStCreateCstringFromAText;
#if IMPLEMENTATION_FOR_WINDOWS
	friend class AStCreateWcharStringFromAText;
#endif
	
	//コンストラクタ／デストラクタ
  public:
	AText( AObjectArrayItem* inParent = NULL );
	AText( AObjectArrayItem* inParent, const AItemCount inInitialAllocateCount, const AItemCount inReallocateStepCount );//B0373
	AText( AConstCharPtr inString );
	AText( const AText& inText );
	~AText();
	
	//代入演算子
  public:
	AText&	operator = ( const AText& inText );
	
	//テキストポインタ
  public:
	//テキストポインタから挿入／追加／設定
	void	InsertFromTextPtr( const AIndex inIndex, const AConstUCharPtr inSrcPtr, const AByteCount inByteCount );
	void	InsertFromTextPtr( const AIndex inIndex, const AConstCharPtr inSrcPtr, const AByteCount inByteCount );
	void	AddFromTextPtr( const AConstUCharPtr inSrcPtr, const AByteCount inByteCount );
	void	AddFromTextPtr( const AConstCharPtr inSrcPtr, const AByteCount inByteCount );
	void	SetFromTextPtr( const AConstUCharPtr inSrcPtr, const AByteCount inByteCount );
	void	SetFromTextPtr( const AConstCharPtr inSrcPtr, const AByteCount inByteCount );
	void	InsertFromUTF16TextPtr( const AIndex inIndex, const AUTF16CharPtr inSrcPtr, const AByteCount inByteCount );
	//テキストポインタへコピー
	void	CopyToTextPtr( AUCharPtr outDstPtr, const AByteCount inMaxByteCount, const AIndex inIndex, const AItemCount inItemCount ) const;
	
	//C文字列
  public:
	//C文字列から挿入／追加／設定
	void	InsertCstring( AIndex inIndex, AConstCstringPtr inString );
	void	SetCstring( const AConstCstringPtr inString );
	void	AddCstring( const AConstCstringPtr inString );
	//printf同等の挿入／追加／設定
	void	SetFormattedCstring( const AConstCstringPtr inFormat, ... );
	void	AddFormattedCstring( const AConstCstringPtr inFormat, ... );
	//C文字列+AText+C文字列の組み合わせ
	void	SetCstringAndText( const AConstCstringPtr inString1, const AText& inText, const AConstCstringPtr inString2 );
  private:
	void	SetFormattedCstring_( const AConstCstringPtr inFormat, va_list inArgList );
	void	AddFormattedCstring_( const AConstCstringPtr inFormat, va_list inArgList );
	
	//AText
  public:
	//ATextから挿入／追加／設定
	void	SetText( const AText& inText );
	void	InsertText( const AIndex inDstIndex, const AText& inSrcText, const AIndex inSrcIndex, const AItemCount inSrcItemCount );
	void	InsertText( const AIndex inDstIndex, const AText& inSrcText );
	void	AddText( const AText& inText );
	//ATextへコピー
	void	GetText( const AIndex& inIndex, const AItemCount& inCount, AText& outText ) const;
	void	GetTextWithoutSpaces( const AIndex inIndex, const AItemCount inCount, AText& outText ) const;//#467
	void	GetTextWithoutSpaces( AText& outText ) const;//#467
	
	//削除
  public:
	/*
	void	DeleteAll();
	void	DeleteAfter( const AIndex inIndex );
	*/
	void	DeleteAfterUTF16NullChar();//win
	
	//比較
  public:
	ABool	Compare( const AText& inTargetText, const AIndex inTargetIndex = 0, const AItemCount inTargetItemCount = kIndex_Invalid ) const;
	ABool	Compare( const AIndex inIndex, const AItemCount inItemCount,
			const AText& inTargetText, const AIndex inTargetIndex, const AItemCount inTargetItemCount ) const;//#695
	ABool	Compare( const AConstCstringPtr inString ) const;
	ABool	CompareMin( const AText& inText ) const;
	ABool	CompareMin( const AConstCstringPtr inString ) const;
	ABool	CompareIgnoreCase( const AText& inText ) const;
	ABool	IsLessThan( const AText& inText ) const;
	//テキスト解析
  public:
	ABool	IsLineEnd( const AIndex inPosition, AIndex& outPosition ) const;
	void	GetLine( AIndex& ioPosition, AText& outText ) const;
	void	GoForwardToNextLineStart( AIndex& ioPosition ) const;
	ABool	GoBackToChar( AIndex& ioPosition, const AUChar inChar ) const;//#427
	ABool	GoForwardToChar( AIndex& ioPosition, const AUChar inChar ) const;//#427
	void	Explode( const AUChar inDelimiter, ATextArray& outTextArray ) const;
	
	void	GetTextToTabSpaceLineEnd( AIndex& ioPos, AText& outText ) const;
	
	//UTF-8文字列操作
  public:
	AIndex	GetPrevCharPos( const AIndex inPos, const ABool inReturnMinusWhen0 = false ) const;//#789
	static AIndex	GetPrevCharPos( const AUChar* inTextPtr, const AItemCount inTextLen,
					const AIndex inPos, const ABool inReturnMinusWhen0 );
	AIndex	GetNextCharPos( const AIndex inPos ) const;
	static AIndex	GetNextCharPos( const AUChar* inTextPtr, const AItemCount inTextLen, const AIndex inPos );
	AIndex	GetCurrentCharPos( const AIndex inPos ) const;
	AItemCount	GetUTF8ByteCount( const AIndex inPos ) const;
	AItemCount	GetUTF8ByteCount( const AUChar inChar ) const;
	ABool	GetUTF8ByteCount( const AUChar inChar, AItemCount& outByteCount ) const;//#412
	void	Get1UTF8Char( AIndex& ioIndex, AText& outChar ) const;
	ABool	IsUTF8FirstByteInMultiByte( const AIndex inPos ) const;
	ABool	IsUTF8FirstByteInMultiByte( const AUChar inChar ) const;
	ABool	IsUTF8MultiByte( const AIndex inPos ) const;
	ABool	IsUTF8MultiByte( const AUChar inChar ) const;
	AItemCount	Convert1CharToUTF16( const AIndex inIndex, AUTF16Char& outUniChar ) const;
	AItemCount	Convert1CharToUCS4( const AIndex inIndex, AUCS4Char& outUniChar ) const;//#412
	static AItemCount	Convert1CharToUCS4( const AUChar* inTextPtr, const AItemCount inTextLen,
				const AIndex inIndex, AUCS4Char& outUniChar );//#733
	void	InsertFromUCS4Char( const AIndex inIndex, const AUCS4Char inUniChar );//#472
	void	AddFromUCS4Char( const AUCS4Char inUniChar );//#472
	
	
	//AHashObjectArrayに保持されるときの検索キーを返すメソッド
	const AText&	GetPrimaryKeyData() const {return *this;}
	
	//正規化
	//　ATextの各種メソッドは基本的に下記のコードであることを前提とする
	//　テキストエンコーディング：UTF-8
	//　改行コード：A_CHAR_LineEnd
	//　そのため解析、編集を行う前に正規化メソッドで上記コードに正規化する。
	void	Normalize();
	void	NormalizeLineEnd();
	
	//挿入
	void	SetTextFromUTF16CharPtr( AUTF16Char* inPtr, unsigned long inLength );//#200 メソッド名変更 AddTextFromUTF16CharPtr→SetTextFromUTF16CharPtr
	void	AddNumber( long inNumber );
	
	void	SetNumber( ANumber inNumber );
	void	AddNumber64bit( const long long inNumber );//#427
	void	SetNumber64bit( const ANumber64bit inNumber );//#427
	
	//テキスト解析
	ABool	ParseIntegerNumber( AIndex& ioPos, ANumber& outNumber, const ABool inAllowMinus = true ) const;
	ABool	ParseIntegerNumber( ANumber& outNumber, const ABool inAllowMinus = true ) const;
	ABool	ParseIntegerNumber64bit( AIndex& ioPos, ANumber64bit& outNumber, const ABool inAllowMinus = true ) const;//#427
	ABool	ParseIntegerNumber64bit( ANumber64bit& outNumber, const ABool inAllowMinus = true ) const;//#427
	ABool	ParseFloatNumber( AIndex& ioPos, AFloatNumber& outNumber, const ABool inAllowMinus = true ) const;
	ABool	ParseFloatNumber( AFloatNumber& outNumber, const ABool inAllowMinus = true ) const;
	ANumber	GetNumber() const;
	ANumber64bit	GetNumber64bit() const;//#427
	AFloatNumber	GetFloatNumber() const;
	
	//参照
	AIndex	GetLength() const { return GetItemCount(); };
	
	//取得
	AUChar	GetLastChar() const;
	
	ANumber	GetIntegerByHexChar( const AIndex inIndex ) const;
	AUInt32	GetIntegerFromHexText() const;
	
	//文字列操作
	void	LimitLength( const AItemCount inMin, const AItemCount inMax );
	void	LimitLengthAsSJIS( const AItemCount& inMin, const AItemCount& inMax );
	void	LimitLengthWithEllipsis( const AItemCount inMax );//#723
	
	void	ChangeCaseLower();
	ABool	ChangeCaseLowerFast();//B0368 B0000
	void	ChangeCaseUpper();
	void	SetRepeatedCstring( const AConstCstringPtr inCstring, const AItemCount inCount );
	void	AddModifiersKeyText( const AModifierKeys inModifierKeys );//#914
	
	//置換
	void	ReplaceParamText( unsigned char inChar, const AText& inText );
	void	ReplaceParamText( const AUChar inChar, const ANumber inNumber );
	void	ReplaceChar( const AUChar inFromChar, const AUChar inToChar );
	void	ReplaceChar( const AUChar inFromChar, AText& inToText );
	void	ReplaceText( const AText& inFromText, const AText& inToText );
	void	ReplaceCstring( const AConstCstringPtr inFromCstring, const AConstCstringPtr inToCstring );
	void	RemoveSpaceTab();//#450
	
	ABool	IsCharContained( unsigned char inChar );
	ABool	IsCharContained( const AUChar inChar, const AIndex inStartIndex, const AIndex inEndIndex ) const;
	
	AItemCount	GetCountOfChar( const AUChar inChar ) const;//#892
	AItemCount	GetCountOfChar( const AUChar inChar, const AIndex inIndex, const AItemCount inCount ) const;//#695
	ABool	Contains( const AConstCstringPtr inStringPtr ) const;//#1050
	
	//文字列置換
	void	ConvertLineEndTo( unsigned char inChar );
	void	ConvertLineEndToLF();
	void	ConvertLineEndToCR();
	
	//テキストエンコーディング／改行コード推測
  public:
	//void	GuessReturnCodeAndWrap( AReturnCode& outReturnCode, ABool& outWrap ) const;
	ABool	GuessTextEncoding( ATextEncoding& ioTextEncoding ) const;//B0390
	ABool	GuessTextEncodingFromCharset( ATextEncoding &outTextEncoding ) const;
	ABool	GuessTextEncodingFromBOM( ATextEncoding &outTextEncoding ) const;
	ABool	GuessTextEncodingLegacy( ATextEncoding &outTextEncoding ) const;
	ABool	GuessTextEncodingByAnalysis( ATextEncoding &outTextEncoding ) const;
	ABool	CalcUTF8ErrorRate( const AItemCount inLimit, AFloatNumber& outErrorRate ) const;
	ABool	CalcEUCErrorRate( const AItemCount inLimit, AFloatNumber& outErrorRate ) const;
	ABool	CalcSJISErrorRate( const AItemCount inLimit, AFloatNumber& outErrorRate ) const;
	
	//テキストエンコーディング／改行コード変換
  public:
	ABool	ConvertToUTF8CR( const ATextEncoding inSrcTextEncoding, const ABool inUseFallback, ABool& outFallbackUsed );
	ABool	ConvertFromUTF8CR( const ATextEncoding inDstTextEncoding, const AReturnCode inDstReturnCode, AText& outText,
							   const ABool inConvertToCanonical = false ) const;//#1044
	AReturnCode	ConvertReturnCodeToCR( const AReturnCode inDefaultReturnCode = returnCode_CR );//#307
	AReturnCode	ConvertReturnCodeToCR( const AReturnCode inDefaultReturnCode, ABool& outNotMixed ); //#995
	void	ConvertReturnCodeFromCR( const AReturnCode inDstReturnCode, AText& outText ) const;
	ABool	ConvertToUTF8( const ATextEncoding inSrcTextEncoding, const ABool inUseFallback, ABool& outFallbackUsed, 
						   const ABool inFromExternalData = false );//B0400
	ABool	ConvertToUTF8( const ATextEncoding inSrcTextEncoding );
	ABool	ConvertFromUTF8( const ATextEncoding inDstTextEncoding, const ABool inConvertToCanonical = false );//#1044
	ABool	CheckUTF8Text( const AItemCount inLimitLength ) const;//#764
	void	ConvertToUTF8FromUTF16();
	ABool	ConvertFromUTF8ToUTF16();//win
	void	GetUTF16TextIndexInfo( AArray<AIndex>& outUTF16PosIndexArray, AArray<AIndex>& outUTF8PosIndexArray ) const;
	
	void	ConvertToUTF8FromAPIUnicode();
	void	ConvertFromUTF8ToAPIUnicode();
	void	ConvertToUTF8FromAPILegacy();
	void	ConvertFromUTF8ToAPILegacy();
	
	//void	NormalizeTextEncodingFromUTF16();
	void	GetUTF16Text( AUniText& outText ) const;
	void	ConvertToCanonicalComp();
	void	ConvertToHFSPlusDecomp();
	
	AUTF16Char	GetUTF16CharFromUTF16Text( const AIndex inIndex ) const;//R0199
	void	InsertUTF16Char( const AIndex inIndex, AUTF16Char inChar );//win
	void	AddUTF16Char( const AUTF16Char inChar )
	{ InsertUTF16Char(GetItemCount(),inChar); }//#863
	
	//
	ABool	SuspectBinaryData() const;
	
	//表示サポート
	void	GetUTF16TextForDisplay( AItemCount inTabWidth, bool inCountAs2Letters0800, bool inDisplayControlCode, AUniText& outText );
	void	GetUTF16TextForDisplayVirtual( AIndex& ioPosition, AItemCount inTabWidth, 
			bool inCountAs2Letters0800, bool inDisplayControlCode, AIndex& ioUniPos, AItemCount& ioTabLetters );
	
	//テキスト解析
	void	GetSuffix( AText& outText ) const;
	AIndex	GetSuffixStartPos() const;
	AIndex	GetFirstSuffixStartPos() const;//R0137
	void	GetToken( AIndex& inPosition, AText& outText ) const;
	static void	GetToken( const AUChar* inTextPtr, const AItemCount inTextLen,
						 AIndex& ioPosition, AText& outText );
	void	GoBackGetToken( AIndex& inPosition, AText& outText ) const;
	static AIndex	GetCharactersOtherThanSpaces( const AUChar* inTextPtr, const AItemCount inTextLen,
												  const AIndex inStartPos, const AItemCount inLength, AText& outText ) ;
	ABool	FindCstring( const AConstCstringPtr inString ) const;//#1231
	ABool	FindCstring( AIndex inPosition, const AConstCstringPtr inString, AIndex& outPosition, 
						const ABool inIgnoreSpace = false, const ABool inProceedPositionAfterMatchedText = false ) const;
	ABool	FindText( AIndex inPosition, const AText& inText, AIndex& outPosition, const AIndex inEndPos = kIndex_Invalid,
					 const ABool inIgnoreSpace = false) const;
	ABool	SkipTabSpace( AIndex& ioPos, const AIndex inEndPos ) const;
	static ABool	SkipTabSpace( const AUChar* inTextPtr, const AItemCount inTextLen,
								 AIndex& ioPos, const AIndex inEndPos ) ;
	ABool	SkipTabSpaceLineEnd( AIndex& ioPos, const AIndex inEndPos ) const;
	ABool	SkipTabSpaceIncludingZenkakuSpace( AIndex& ioPos, const AIndex inEndPos ) const;//#318
	void	SetZenkakuSpace();//#318
	ABool	CompareZenkakuSpace() const;//#942
	
	//
	void	CalcParagraphBreaks( AArray<AIndex>& outStartPosArray, AArray<AItemCount>& outLengthArray ) const;
	
	//正規表現による処理 #193
	ABool	ExtractUsingRegExp( const AConstCharPtr inRegExpString, const AIndex inExtractGroup, AText& outText ) const;
	
	//Localized Text
	void	SetLocalizedText( const AText& inKey );
	void	SetLocalizedText( AConstCharPtr inKey );
	void	SetLocalizedText( AConstCharPtr inKey, const ANumber inNumber );
	void	AddLocalizedText( AConstCharPtr inKey );
	
	//FTPURL
	void	SetFTPURL( const AText& inConnectionType, const AText& inServer, const AText& inUser, const AText& inPath, const APortNumber inPortNumber );//#193
	void	GetFTPURLProtocol( AText& outProtocol ) const;//#193
	void	GetFTPURLServer( AText& outServer ) const;
	void	GetFTPURLUser( AText& outUser ) const;
	void	GetFTPURLPath( AText& outPath ) const;
	APortNumber	GetFTPURLPortNumber() const;//#193
	void	CatFTPURLPathText( const AText& inRelativePath );
	void	GetFilename( AText& outFilename ) const;
	
	//ファイルパス用テキスト処理
  public:
	void	RemoveLastPathDelimiter();//#892
	void	AddLastPathDelimiter();//#892
	
	//URL
  public:
	void	SetChildURL( const AText& inParentURL, const AText& inChildName );//#427
	void	SetChildURL( const AText& inParentURL, const AConstCstringPtr inChildName );//#427
	ABool	GetParentURL( AText& outParentURL ) const;//#631
	void	ConvertToURLEncode();//#363
	void	ConvertToURLEscape();//R0000
	void	ConvertFromURLEscape();//#358
	
	void	AddPathDelimiter( const AUChar inDelimiter );//win 080713
	
	//pref text
  public:
	void	ConvertToPrefTextEscape();
	void	ExtractUnescapedTextFromPrefText( AIndex& ioPos, AText& outText, const AUChar inTerminateChar ) const;
	
	//汎用escaped text
  public:
	void	ConvertToEscapedText( const ABool inEscapeLineEnd, const ABool inEscapeDoubleQuotation );
	void	ConvertToUnescapedText();
	
	//正規表現用escaped text
  public:
	void	ConvertToRegExpEscapedText();
	
	//Cocoa
  public:
	NSString*			CreateNSString( const ABool inAutorelease ) const;//#1034
	void				SetFromNSString( const NSString* inNSString );//#1034
	
	//デバッグ
	void	OutputToStderr() const;
	void	OutputToStderrHex() const;
	
#if IMPLEMENTATION_FOR_MACOSX
	//Mac専用メソッド
  public:
	AText( Str255 inString );
	
	void	GetPstring( Str255 outString, const ANumber& inMaxLength ) const;
	void	SetPstring( const Str255 inString );
	
	OSType	GetOSTypeFromText() const;
	void	SetFromOSType( const OSType inOSType );
	
	void	SetFromCFString( const CFStringRef inString );
	
	CFStringRef	CreateCFString() const;
#endif
#if IMPLEMENTATION_FOR_WINDOWS
  public:
	void	SetFromWcharString( const wchar_t* inString, AItemCount inMaxLength );
#endif
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "AText"; }
	
};



#pragma mark ===========================
#pragma mark [クラス]AStCreateCstringFromAText
//ATextからC文字列ポインタを取得するためのクラス
//（スタック生成専用クラス）
class AStCreateCstringFromAText : public AStackOnlyObject
{
	//コンストラクタ
  public:
	//生成時にATextが確定できる場合用
	AStCreateCstringFromAText( const AText& inText );
	
	//生成時にATextが確定できない場合用
	AStCreateCstringFromAText();
	void	Create( const AText& inText );
	
	//デストラクタ
	~AStCreateCstringFromAText();
	
	//ポインタの取得
	ACstringPtr	GetPtr();
	
  private:
	AText	mCstringText;
	
};

#if IMPLEMENTATION_FOR_WINDOWS
#pragma mark ===========================
#pragma mark [クラス]AStCreateWcharStringFromAText
//ATextからwchar_t文字列ポインタを取得するためのクラス
//（スタック生成専用クラス）
class AStCreateWcharStringFromAText : public AStackOnlyObject
{
	//コンストラクタ
  public:
	//生成時にATextが確定できる場合用
	AStCreateWcharStringFromAText( const AText& inText );
	
	//生成時にATextが確定できない場合用
	AStCreateWcharStringFromAText();
	void	Create( const AText& inText );
	
	//デストラクタ
	~AStCreateWcharStringFromAText();
	
	//ポインタの取得
	const wchar_t*	GetPtr();
	wchar_t*	GetWritablePtr();
	
	//文字数の取得
	int	GetWcharCount() const;
	
  private:
	AText	mWcharText;
	
};
#endif

#if IMPLEMENTATION_FOR_MACOSX
#pragma mark ===========================
#pragma mark [クラス]AStCreateCFStringFromAText
//ATextからCFStringを取得するためのクラス(Mac専用)
//（スタック生成専用クラス）
class AStCreateCFStringFromAText : public AStackOnlyObject
{
	//コンストラクタ／デストラクタ
  public:
	AStCreateCFStringFromAText( const AText& inText );
	~AStCreateCFStringFromAText();
	
	//CFStringRef取得
	CFStringRef	GetRef();
	
  private:
	CFStringRef	mStringRef;
};
#pragma mark ===========================
#pragma mark [クラス]AStCreateCFMutableStringFromAText
//ATextからCFStringを取得するためのクラス(Mac専用)
//（スタック生成専用クラス）
class AStCreateCFMutableStringFromAText : public AStackOnlyObject
{
	//コンストラクタ／デストラクタ
  public:
	AStCreateCFMutableStringFromAText( const AText& inText );
	~AStCreateCFMutableStringFromAText();
	
	//CFStringRef取得
	CFMutableStringRef	GetRef();
	
  private:
	CFMutableStringRef	mStringRef;
};
#endif 

class AUniText : public AArray<AUTF16Char>
{
	//friend class AStUniTextPtr;
  public:
	AUniText() {}
	
	//★
	void	InsertFromPtr( AIndex inIndex, AUTF16CharPtr inSrcPtr, AItemCount inItemCount );

};

#define IsUTF8Multibyte(c) ((c&0x80)!=0)
#define IsUTF8Multibyte1(c) ((c&0x80)!=0&&(c&0xC0)!=0x80)
#define IsUTF8Multibyte23(c) ((c&0xC0)==0x80)

