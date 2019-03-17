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

ABase
AbsBase部全体を定義するヘッダファイル

*/

#pragma once

/*#205
コンパイルオプションによる切り替えをやめ、コントロールごとにタイプを切り替えられるようにした。
kDefaultTableViewTypeにて、タイプ省略時のデフォルトを設定可能。
AWindowクラスを参照のこと。
#if IMPLEMENTATION_FOR_MACOSX
#define USE_IMP_TABLEVIEW 1
#else
#define USE_IMP_TABLEVIEW 0
#endif
*/

//基本データ型
#include "ABaseTypes.h"
//基本パラメータ値
#include "ABaseParameters.h"
//基本ラッパー
#include "../AbsBase_Imp/ABaseWrapper.h"

//ABaseオブジェクト操作関数
#include "ABaseFunction.h"

//全オブジェクトの基底クラス３種
#include "ASingletonObject.h"
#include "AStackOnlyObject.h"
#include "AObjectArrayItem.h"

//可変長配列（基本データ型）
#include "AArray.h"
#include "AHashArray.h"
//可変長配列（オブジェクト）
#include "AObjectArray.h"
#include "AObjectArrayIDIndexed.h"//#693
#include "AHashObjectArray.h"
//
//#693 #include "APtrManager.h"
//テキスト
#include "AText.h"
//テキスト配列
#include "ATextArray.h"

typedef AHashArray<ANumber> AHashNumberArray;

//スレッド
//#include "AThread.h"
#include "AThreadMutex.h"

//
#include "AObjectArrayItemTrash.h"

//内部イベントキュー
#include "AInternalEvent.h"

//ファイル
#include "AFileAcc.h"
//簡易データベース
#include "ADataBase.h"

//正規表現
#include "ARegExp.h"

//zipファイル #427
#include "AZipFile.h"

//テキストエンコーディングラッパー
#include "../AbsBase_Imp/ATextEncodingWrapper.h"

//DateTime
#include "../AbsBase_Imp/ADateTimeWrapper.h"

//Font
#include "../AbsBase_Imp/AFontWrapper.h"

//トレース
#include "ATrace.h"

//CSVファイル #791
#include "ACSVFile.h"

class AApplicationFactory;

/**
ABaseオブジェクト（必ずアプリ起動直後に生成必要）

AbsFrame内の各クラス（AApplication等）が無くても、
独立してAText等のABase内の各クラスが使用できるように、
こちらで各初期化を行う。
ただ、まだ完全に独立化出来ていない。当面はAbsFrameとセットで使用する。
*/
class ABase : public ASingletonObject
{
	//ABaseFunctionクラスはAArrayのLock/Unlock等を行う関数を定義している。
	//これらの関数は、AArray等のオブジェクトからコールされるが、それらのクラスはtemplateで定義されており、
	//定義と宣言を分離することが出来ないことから、それらのAArrayオブジェクトをABaseオブジェクトがコンポジションしているときに、
	//これらの関数をABaseのメソッドとして定義してしまうと、循環定義が発生してしまう。
	//これを回避するために、これらの関数をフレンドクラスに分離している。これにより、AArray等の定義の中では、ABaseFunctionクラスのみをincludeすれば良い。
	friend class ABaseFunction;
	
	//コンストラクタ／デストラクタ
  public:
	ABase( const ABool& inUseTraceLog );
	~ABase();
	
	//
  public:
	ABool	InMainThread() const;
	//メインスレッドID
  private:
	AThreadID	mMainThreadID;
	
	//メインスレッドのAObjectArrayItemゴミ箱
  private:
	AObjectArrayItemTrash	mObjectArrayItemTrashForMainThread;
	
	//メイン内部イベントキュー
  public:
	//ABaseFunctionに入れると、AObjectIDArrayの定義でうまくいかないので、こちらに入れます・・・
	static ABool	DequeueFromMainInternalEventQueue( AInternalEventType& outType, AObjectID& outSenderObjectID,
					ANumber& outNumber, AText& outText, AObjectIDArray& outObjectIDArray );
	static AItemCount	GetMainInternalEventQueueCount();
	static void	PostToMainInternalEventQueue( const AInternalEventType& inType, const AObjectID inSenderObjectID,
				const ANumber& inNumber, const AText& inText, const AObjectIDArray& inObjectIDArray );
	static void	RemoveEventFromMainInternalEventQueue( const AInternalEventType inType, const AObjectID inSenderObjectID );//#698
	static ABool ExistEventInInternalEventQueue( const AInternalEventType inType );//#1056
  private:
	AInternalEventQueue	mMainInternalEventQueue;
	
	/*#344 メモリープール機構削除（仮想メモリ環境では使用される状況がまずありえないので）
	//メモリ不足時用メモリプール
  private:
	void*	mLevel1MemoryPool;
	void*	mLevel2MemoryPool;
	*/
	
	//トレース
  private:
	ABool	mUseTraceLog;
};

#include "AStArrayPtr.h"

//空のオブジェクト取得
extern const AText&	GetEmptyText();
extern const ATextArray&	GetEmptyTextArray();
extern const ABoolArray&	GetEmptyBoolArray();
extern const ANumberArray&	GetEmptyNumberArray();
extern const AColorArray&	GetEmptyColorArray();
extern const AObjectIDArray&	GetEmptyObjectIDArray();
extern const AHashNumberArray&	GetEmptyHashNumberArray();

#include "AStEventTransactionPrePostProcess.h"

#pragma mark ===========================

#pragma mark ---Rect/Point

//比較
#define ComparePoint(a,b) ((((a).x)==((b).x))&&(((a).y)==((b).y)))

//APointユーティリティ
extern void	OrderPoint( const APoint& inSelectTextPoint1, const APoint& inSelectTextPoint2, APoint& outStartTextPoint, APoint& outEndTextPoint );

//#232 typedef ADataID	ATextArrayMenuID;
struct AImagePoint
{
	ANumber	x;
	ANumber	y;
};
extern void	OrderImagePoint( const AImagePoint& inSelectTextPoint1, const AImagePoint& inSelectTextPoint2, AImagePoint& outStartTextPoint, AImagePoint& outEndTextPoint );

extern void	OrderTextPoint( const ATextPoint& inSelectTextPoint1, const ATextPoint& inSelectTextPoint2, ATextPoint& outStartTextPoint, ATextPoint& outEndTextPoint );//win 080618

const AImagePoint kImagePoint_00 = {0,0};

//#1316 ANumber→AFloatNumber
struct ALocalPoint
{
	AFloatNumber	x;
	AFloatNumber	y;
};

struct AWindowPoint
{
	ANumber	x;
	ANumber	y;
};

struct AGlobalPoint
{
	ANumber	x;
	ANumber	y;
};

const AWindowPoint	kWindowPoint_00 = {0,0};//#379
const ALocalPoint	kLocalPoint_00 = {0,0};//#379
const AGlobalPoint	kGlobalPoint_00 = {0,0};//#634

struct AImageRect
{
	ANumber	left;
	ANumber	top;
	ANumber	right;
	ANumber	bottom;
};

//#1316 ANumber→AFloatNumber
struct ALocalRect
{
	AFloatNumber	left;
	AFloatNumber	top;
	AFloatNumber	right;
	AFloatNumber	bottom;
};

struct AWindowRect
{
	ANumber	left;
	ANumber	top;
	ANumber	right;
	ANumber	bottom;
};

struct AGlobalRect
{
	ANumber	left;
	ANumber	top;
	ANumber	right;
	ANumber	bottom;
};

const AWindowRect	kWindowRect_0000 = {0,0,0,0};//#688
const ALocalRect	kLocalRect_0000 = {0,0,0,0};//#688
const AGlobalRect	kGlobalRect_0000 = {0,0,0,0};//#688
const ARect	kRect_0000 = {0,0,0,0};

//#850
ABool	IsDragStart( const ALocalPoint& inCurrentPoint, const ALocalPoint& inOriginalPoint );

#pragma mark ===========================

#pragma mark ---Rect/Pointユーティリティ

/**
PointがRect内にあるかどうかを判定
*/
#define PointInRect(inPoint,inRect) ( inPoint.x >= inRect.left && inPoint.x <= inRect.right && inPoint.y >= inRect.top && inPoint.y <= inRect.bottom )


#pragma mark ===========================

#pragma mark ---キーバインド

enum AKeyBindKey
{
	kKeyBindKey_Invalid = -1,
	kKeyBindKey_A = 0,
	kKeyBindKey_B,
	kKeyBindKey_C,
	kKeyBindKey_D,
	kKeyBindKey_E,
	kKeyBindKey_F,
	kKeyBindKey_G,
	kKeyBindKey_H,
	kKeyBindKey_I,
	kKeyBindKey_J,
	kKeyBindKey_K,
	kKeyBindKey_L,
	kKeyBindKey_M,
	kKeyBindKey_N,
	kKeyBindKey_O,
	kKeyBindKey_P,
	kKeyBindKey_Q,
	kKeyBindKey_R,
	kKeyBindKey_S,
	kKeyBindKey_T,
	kKeyBindKey_U,
	kKeyBindKey_V,
	kKeyBindKey_W,
	kKeyBindKey_X,
	kKeyBindKey_Y,
	kKeyBindKey_Z,
	kKeyBindKey_F1,
	kKeyBindKey_F2,
	kKeyBindKey_F3,
	kKeyBindKey_F4,
	kKeyBindKey_F5,
	kKeyBindKey_F6,
	kKeyBindKey_F7,
	kKeyBindKey_F8,
	kKeyBindKey_F9,
	kKeyBindKey_F10,
	kKeyBindKey_F11,
	kKeyBindKey_F12,
	kKeyBindKey_F13,
	kKeyBindKey_F14,
	kKeyBindKey_F15,
	kKeyBindKey_Return,
	kKeyBindKey_Tab,
	kKeyBindKey_Space,
	kKeyBindKey_Escape,
	kKeyBindKey_Enter,
	kKeyBindKey_BS,
	kKeyBindKey_DEL,
	kKeyBindKey_Up,
	kKeyBindKey_Down,
	kKeyBindKey_Left,
	kKeyBindKey_Right,
	kKeyBindKey_Colon,
	kKeyBindKey_Semicolon,
	kKeyBindKey_Minus,
	kKeyBindKey_Hat,
	kKeyBindKey_Backslash,
	kKeyBindKey_Atmark,
	kKeyBindKey_BracketStart,
	kKeyBindKey_BracketEnd,
	kKeyBindKey_Comma,
	kKeyBindKey_Period,
	kKeyBindKey_Slash,//ここまでが2.1.8a7時点での最後のキーバインド(62)
	kKeyBindKey_Home,
	kKeyBindKey_End,
	kKeyBindKey_PageUp,
	kKeyBindKey_PageDown,
	kKeyBindKey_1,//#915
	kKeyBindKey_2,
	kKeyBindKey_3,
	kKeyBindKey_4,
	kKeyBindKey_5,
	kKeyBindKey_6,
	kKeyBindKey_7,
	kKeyBindKey_8,
	kKeyBindKey_9,
	kKeyBindKey_0,
	kKeyBindKeyItemCount
};

//#688
//キーバインドアクション
typedef AIndex AKeyBindAction;

const AKeyBindAction keyAction_InsertText = -2;
const AKeyBindAction keyAction_CannotChangeBind = -1;
const AKeyBindAction keyAction_NOP = 0;
const AKeyBindAction keyAction_string = 1;
const AKeyBindAction keyAction_return = 2;
const AKeyBindAction keyAction_indentreturn = 3;
const AKeyBindAction keyAction_tab = 4;
const AKeyBindAction keyAction_indenttab = 5;
const AKeyBindAction keyAction_space = 6;
const AKeyBindAction keyAction_abbrevnext = 7;
const AKeyBindAction keyAction_abbrevprev = 8;
const AKeyBindAction keyAction_deletenext = 9;
const AKeyBindAction keyAction_deleteprev = 10;
const AKeyBindAction keyAction_deletelineend = 11;
const AKeyBindAction keyAction_caretup = 12;
const AKeyBindAction keyAction_caretdown = 13;
const AKeyBindAction keyAction_caretleft = 14;
const AKeyBindAction keyAction_caretright = 15;
const AKeyBindAction keyAction_carethome = 16;
const AKeyBindAction keyAction_caretend = 17;
const AKeyBindAction keyAction_caretlinestart = 18;
const AKeyBindAction keyAction_caretlineend = 19;
const AKeyBindAction keyAction_selectup = 20;
const AKeyBindAction keyAction_selectdown = 21;
const AKeyBindAction keyAction_selectleft = 22;
const AKeyBindAction keyAction_selectright = 23;
const AKeyBindAction keyAction_pageup = 24;
const AKeyBindAction keyAction_pagedown = 25;
const AKeyBindAction keyAction_home = 26;
const AKeyBindAction keyAction_end = 27;
const AKeyBindAction keyAction_scrollup = 28;
const AKeyBindAction keyAction_scrolldown = 29;
const AKeyBindAction keyAction_scrollright = 30;
const AKeyBindAction keyAction_scrollleft = 31;
const AKeyBindAction keyAction_previousword = 32;
const AKeyBindAction keyAction_nextword = 33;
const AKeyBindAction keyAction_pageupwithcaret = 34;
const AKeyBindAction keyAction_pagedownwithcaret = 35;
const AKeyBindAction keyAction_Undo = 36;
const AKeyBindAction keyAction_Cut = 37;
const AKeyBindAction keyAction_Copy = 38;
const AKeyBindAction keyAction_Paste = 39;
const AKeyBindAction keyAction_cutlineend = 40;
const AKeyBindAction keyAction_Redo = 41;
const AKeyBindAction keyAction_selectnextword = 42;
const AKeyBindAction keyAction_selectprevword = 43;
const AKeyBindAction keyAction_correspondence = 44;
const AKeyBindAction keyAction_recenter = 45;
const AKeyBindAction keyAction_selecthome = 46;
const AKeyBindAction keyAction_selectend = 47;
const AKeyBindAction keyAction_selectlinestart = 48;
const AKeyBindAction keyAction_selectlineend = 49;
const AKeyBindAction keyAction_selectmode = 50;
const AKeyBindAction keyAction_cutlinestart = 51;
const AKeyBindAction keyAction_selectbrace = 52;
const AKeyBindAction keyAction_sameletterup = 53;
const AKeyBindAction keyAction_duplicateline = 54;
const AKeyBindAction keyAction_insertline = 55;
const AKeyBindAction keyAction_previousword_linestop = 56;//R0052
const AKeyBindAction keyAction_nextword_linestop = 57;//R0052
const AKeyBindAction keyAction_selectpreviousword_linestop = 58;//R0063
const AKeyBindAction keyAction_selectnextword_linestop = 59; //R0063
const AKeyBindAction keyAction_caretparagraphstart = 60;//R0147
const AKeyBindAction keyAction_caretparagraphend = 61;//R0147
const AKeyBindAction keyAction_deleteparagraphend = 62;//R0213
const AKeyBindAction keyAction_cutparagraphend = 63;//R0213
const AKeyBindAction keyAction_additionalcutparagraphend = 64;//R0213
const AKeyBindAction keyAction_additionalcut = 65;//R0213
const AKeyBindAction keyAction_additionalcopy = 66;//R0213
const AKeyBindAction keyAction_prevdiff = 67;//R0006
const AKeyBindAction keyAction_nextdiff = 68;//R0006
const AKeyBindAction keyAction_idinfo = 69;//RC2
const AKeyBindAction keyAction_deletewholeparagraph = 70;//#124
const AKeyBindAction keyAction_indent = 71;//#249
const AKeyBindAction keyAction_tab_byspace = 72;//#249
const AKeyBindAction keyAction_indent_byspace_tab = 73;//#249
const AKeyBindAction keyAction_indent_byspace_tab_byspace = 74;//#249
const AKeyBindAction keyAction_indent_byspace = 75;//#249
const AKeyBindAction keyAction_indent_byspace_return = 76;//#249
const AKeyBindAction keyAction_indent_onlynewline_return = 77;//#249
const AKeyBindAction keyAction_indent_onlynewline_byspace_return = 78;//#249
const AKeyBindAction keyAction_nextheader = 79;//#150
const AKeyBindAction keyAction_prevheader = 80;//#150
const AKeyBindAction keyAction_switch_nexttab = 81;//#357
const AKeyBindAction keyAction_switch_prevtab = 82;//#357
const AKeyBindAction keyAction_linedialog = 83;//#594
const AKeyBindAction keyAction_autoindentmode = 84;//#638
//#688
const AKeyBindAction keyAction_cancelOperation = 85;//未対応
const AKeyBindAction keyAction_capitalizeWord = 86;//未対応
const AKeyBindAction keyAction_changeCaseOfLetter = 87;//未対応
const AKeyBindAction keyAction_deleteBackwardByDecomposingPreviousCharacter = 88;
const AKeyBindAction keyAction_deletelinestart = 89;
const AKeyBindAction keyAction_deleteparagraphstart = 90;
const AKeyBindAction keyAction_deleteToMark = 91;
const AKeyBindAction keyAction_deleteWordBackward = 92;//未対応
const AKeyBindAction keyAction_deleteWordForward = 93;//未対応
const AKeyBindAction keyAction_insertContainerBreak = 94;//未対応
const AKeyBindAction keyAction_insertLineBreak = 95;//未対応
const AKeyBindAction keyAction_lowercaseWord = 96;//未対応
const AKeyBindAction keyAction_selectparagraphstart = 97;
const AKeyBindAction keyAction_pageDownAndModifySelection = 98;//未対応
const AKeyBindAction keyAction_pageUpAndModifySelection = 99;//未対応
const AKeyBindAction keyAction_selectAll = 100;
const AKeyBindAction keyAction_selectLine = 101;
const AKeyBindAction keyAction_selectParagraph = 102;
const AKeyBindAction keyAction_selectToMark = 103;
const AKeyBindAction keyAction_selectWord = 104;
const AKeyBindAction keyAction_setMark = 105;
const AKeyBindAction keyAction_swapWithMark = 106;
const AKeyBindAction keyAction_uppercaseWord = 107;//未対応
const AKeyBindAction keyAction_yank = 108;
const AKeyBindAction keyAction_selectparagraphend = 109;
//マクロバー項目へのキーバインド #918
const AKeyBindAction keyAction_macro1 = 110;
const AKeyBindAction keyAction_macro2 = 111;
const AKeyBindAction keyAction_macro3 = 112;
const AKeyBindAction keyAction_macro4 = 113;
const AKeyBindAction keyAction_macro5 = 114;
const AKeyBindAction keyAction_macro6 = 115;
const AKeyBindAction keyAction_macro7 = 116;
const AKeyBindAction keyAction_macro8 = 117;
const AKeyBindAction keyAction_macro9 = 118;
//#815
const AKeyBindAction keyAction_shiftleft = 119;
const AKeyBindAction keyAction_shiftright = 120;
//#1154
const AKeyBindAction keyAction_swapLetter = 121;
//#1397
const AKeyBindAction keyAction_findnext = 122;
const AKeyBindAction keyAction_findprev = 123;
//#1399
const AKeyBindAction keyAction_caretprevparagraphstart = 124;
const AKeyBindAction keyAction_caretnextparagraphend = 125;
const AKeyBindAction keyAction_selectprevparagraphstart = 126;
const AKeyBindAction keyAction_selectnextparagraphend = 127;

//#688 const AItemCount keyActionItemCount = 85;

enum AJustification
{
	kJustification_Left = 0,
	kJustification_Right,
	kJustification_Center,
	kJustification_LeftTruncated,
	kJustification_CenterTruncated,
	kJustification_RightCenter
};

enum AScrollBarPart
{
	kScrollBarPart_UpButton = 0,
	kScrollBarPart_DownButton,
	kScrollBarPart_PageUpButton,
	kScrollBarPart_PageDownButton,
	kScrollBarPart_ThumbTracking,
	kScrollBarPart_None
};

#pragma mark ---ユーティリティ
//#688

//ANumberユーティリティ
ANumber	GetMinValue( const ANumber inNumber1, const ANumber inNumber2 );
ANumber	GetMaxValue( const ANumber inNumber1, const ANumber inNumber2 );

//ALocalRectユーティリティ
ABool	GetIntersectRect( const ALocalRect& inRect1, const ALocalRect& inRect2, ALocalRect& outIntersectRect );
ABool	IsPointInRect( const ALocalPoint inPoint, const ALocalRect& inRect );

//AbsBaseで使用するImageID #688
//const AImageID	kImageID_SizeBox				= 12;
const AImageID	kImageID_iconSwCancel 			= 13;
const AImageID	kImageID_iconSwCancel_h			= 14;
const AImageID	kImageID_iconSwCancel_p			= 15;
const AImageID	kImageID_iconSwSearch			= 16;
/*#1316
//Rect20ボタンImage
const AImageID	kImageID_btn20Rect_1			= 40;
const AImageID	kImageID_btn20Rect_2			= 41;
const AImageID	kImageID_btn20Rect_3			= 42;
const AImageID	kImageID_btn20Rect_h_1			= 43;
const AImageID	kImageID_btn20Rect_h_2			= 44;
const AImageID	kImageID_btn20Rect_h_3			= 45;
const AImageID	kImageID_btn20Rect_p_1			= 46;
const AImageID	kImageID_btn20Rect_p_2			= 47;
const AImageID	kImageID_btn20Rect_p_3			= 48;
//RoundedRect20ボタンImage
const AImageID	kImageID_btn20RoundedRect_1			= 50;
const AImageID	kImageID_btn20RoundedRect_2			= 51;
const AImageID	kImageID_btn20RoundedRect_3			= 52;
const AImageID	kImageID_btn20RoundedRect_h_1		= 53;
const AImageID	kImageID_btn20RoundedRect_h_2		= 54;
const AImageID	kImageID_btn20RoundedRect_h_3		= 55;
const AImageID	kImageID_btn20RoundedRect_p_1		= 56;
const AImageID	kImageID_btn20RoundedRect_p_2		= 57;
const AImageID	kImageID_btn20RoundedRect_p_3		= 58;
//Rect16ボタンImage
const AImageID	kImageID_btn16Rect_1			= 60;
const AImageID	kImageID_btn16Rect_2			= 61;
const AImageID	kImageID_btn16Rect_3			= 62;
const AImageID	kImageID_btn16Rect_h_1			= 63;
const AImageID	kImageID_btn16Rect_h_2			= 64;
const AImageID	kImageID_btn16Rect_h_3			= 65;
const AImageID	kImageID_btn16Rect_p_1			= 66;
const AImageID	kImageID_btn16Rect_p_2			= 67;
const AImageID	kImageID_btn16Rect_p_3			= 68;
//Rect16FooterボタンImage
const AImageID	kImageID_panel16Footer_1		= 70;
const AImageID	kImageID_panel16Footer_2		= 71;
const AImageID	kImageID_panel16Footer_3		= 72;
//リスト項目Image
const AImageID	kImageID_iconSwFile				= 80;
//
const AImageID	kImageID_frameScrlbarTop_106	= 85;
const AImageID	kImageID_frameScrlbarTop_107	= 86;
*/
//ポップアップメニュー用Image
const AImageID	kImageID_iconSwTriangles		= 90;
//
const AImageID	kImageID_barSwCursorRight		= 91;
const AImageID	kImageID_barSwCursorRight_h		= 92;
const AImageID	kImageID_barSwCursorRight_p		= 93;
const AImageID	kImageID_barSwCursorDown		= 94;
const AImageID	kImageID_barSwCursorDown_h		= 95;
const AImageID	kImageID_barSwCursorDown_p		= 96;



//#725
/**
EditBoxタイプ
*/
enum AEditBoxType
{
	kEditBoxType_Normal = 0,
	kEditBoxType_FilterBox,
	kEditBoxType_ToolTip,//#1316 kEditBoxType_ThinFrame→kEditBoxType_ToolTip
	kEditBoxType_NoFrameDraw,
};

//#725
/**
FrameViewタイプ
*/
enum AFrameViewType
{
	kFrameViewType_ToolTip = 0,//#1316 kFrameViewType_Normal→kFrameViewType_ToolTip
	kFrameViewType_EditBox,
	kFrameViewType_FilterBox,
	kFrameViewType_NoFrameDraw,
};

#define BUILD_FOR_MACOSX_10_3 0
#define BUILD_WITH_JAVASCRIPTCORE 1
