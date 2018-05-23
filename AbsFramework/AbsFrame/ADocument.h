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

ADocument

*/

#pragma once

#include "../AbsBase/ABase.h"
#include "AbsFrame.h"
#include "../Imp.h"
//#include "../CPrintImp.h"

class AApplication;

/**
ドキュメントの基底クラス
*/
//AApplicationにAObjectArray<ADocument>がメンバとして保持される。このため、純仮想メソッド使用不可。
class ADocument: public AObjectArrayItem
{
	//コンストラクタ、デストラクタ
  public:
	ADocument( AObjectArrayItem* inParent, const AObjectID inDocImpID );//#1034
	virtual ~ADocument();
	void					NVI_DocumentWillBeDeleted();//#92
  private:
	virtual void			NVIDO_DocumentWillBeDeleted() {}//#92
	
	//CDocImp #1034
  public:
	AObjectID				NVI_GetDocImpID() const { return mDocImpID; }
  private:
	AObjectID							mDocImpID;
	
	//<イベント処理>
  public:
	ABool					EVT_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys )
			{ return EVTDO_DoMenuItemSelected(inMenuItemID,inDynamicMenuActionText,inModifierKeys); }
	void					EVT_UpdateMenu()
			{ EVTDO_UpdateMenu(); }
	AItemCount				EVT_GetPrintPageCount() { return EVTDO_GetPrintPageCount(); }
	void					EVT_PrintPage( const AIndex inPageIndex ) { return EVTDO_PrintPage(inPageIndex); }
	void					EVT_StartPrintMode( const ALocalRect& inRect, const ANumber inPaperWidth, const ANumber inPaperHeight ) { mPrintMode = true; EVTDO_StartPrintMode(inRect,inPaperWidth,inPaperHeight); }//#558
	void					EVT_EndPrintMode() { EVTDO_EndPrintMode(); mPrintMode = false; }
	ABool					NVI_GetPrintMode() const { return mPrintMode; }
  private:
	virtual ABool			EVTDO_DoMenuItemSelected( const AMenuItemID inMenuItemID, const AText& inDynamicMenuActionText, const AModifierKeys inModifierKeys ) {return false;}
	virtual void			EVTDO_UpdateMenu() {}
	virtual AItemCount		EVTDO_GetPrintPageCount() { return 0; }
	virtual void			EVTDO_PrintPage( const AIndex inPageIndex ) {}
	virtual void			EVTDO_StartPrintMode( const ALocalRect& inRect, const ANumber inPaperWidth, const ANumber inPaperHeight ) {}//#558
	virtual void			EVTDO_EndPrintMode() {}
	ABool								mPrintMode;
	
	//<インターフェイス>
	//ドキュメント情報設定／取得
  public:
	ADocumentType			NVI_GetDocumentType() const { return NVIDO_GetDocumentType(); }
	void					NVI_GetTitle( AText& outTitle ) const { NVIDO_GetTitle(outTitle); }
	ABool					NVI_GetFile( AFileAcc& outFile ) const;
	void					NVI_GetFilePath( AText& outFilePath ) const;//#723
	ABool					NVI_GetParentFolder( AFileAcc& outFolder ) const;//#465
	void					NVI_SpecifyFile( const AFileAcc& inFileAcc );
	void					NVI_SpecifyFileUnscreened( const AFileAcc& inFileAcc );
	ABool					NVI_IsFileSpecified() const;
	ABool					NVI_IsFileSpecifiedAndScreened() const { return mFileSpecifiedAndScreened; }
	void					NVI_SetDirty( const ABool inDirty );
	ABool					NVI_IsDirty() const;
	void					NVI_SetAlwaysNotDirty();//#567
	void					NVI_SetReadOnly( const ABool inReadOnly );
	ABool					NVI_IsReadOnly() const { return NVIDO_IsReadOnly(); }//#699
	//#182 void					NVI_UpdateProperty();
	void					NVI_GetPrintMargin( AFloatNumber& outLeftMargin, AFloatNumber& outRightMargin, AFloatNumber& outTopMargin, AFloatNumber& outBottomMargin ) const
	{ NVIDO_GetPrintMargin(outLeftMargin,outRightMargin,outTopMargin,outBottomMargin); }//R0242
  private:
	virtual ADocumentType	NVIDO_GetDocumentType() const { return kDocumentType_Invalid; }
	virtual void			NVIDO_GetTitle( AText& outTitle ) const { _ACError("not impletemted",this); }
	virtual void			NVIDO_SpecifyFile( const AFileAcc& inFileAcc ) {}
	virtual ABool			NVIDO_IsReadOnly() const { return mReadOnly; }//#699
	//#182 virtual void			NVIDO_UpdateProperty() {}
	//R0242
	virtual void			NVIDO_GetPrintMargin( AFloatNumber& outLeftMargin, AFloatNumber& outRightMargin, AFloatNumber& outTopMargin, AFloatNumber& outBottomMargin ) const
	{
		outLeftMargin	= 10.0;
		outRightMargin	= 10.0;
		outTopMargin	= 10.0;
		outBottomMargin	= 10.0;
	}
	virtual void			NVIDO_SetDirty( const ABool inDirty ) {}
  protected:
	AFileAcc							mFileAcc;
	ABool								mDirty;
	ABool								mReadOnly;
	ABool								mAlwaysNotDirty;//#567
	ABool								mFileSpecifiedAndScreened;
	
	/*#379
	//ウインドウ
  protected:
	AArray<AObjectID>					mWindowIDArray;
	*/
	
	//#379
	//非同期プロセス実行・結果実行
  public:
	void					NVI_DoGeneralAsyncCommandResult( const AGeneralAsyncCommandType inType, const AText& inText )
	{ NVIDO_DoGeneralAsyncCommandResult(inType,inText); }
  private:
	virtual void			NVIDO_DoGeneralAsyncCommandResult( const AGeneralAsyncCommandType inType, const AText& inText ) {}
	
	//#379
  public:
	void					NVI_Retain() { mRetainCount++; }
	void					NVI_Release();
  private:
	AItemCount								mRetainCount;
	
	//View #92
  public:
	void					NVI_RegisterView( const AViewID inViewID );
	void					NVI_UnregisterView( const AViewID inViewID );
	void					NVI_RevealDocumentWithAView() const;
  protected:
	AArray<AViewID>						mViewIDArray;
  private:
	virtual void			NVIDO_ViewRegistered( const AViewID inViewID ) {}//#379
	virtual void			NVIDO_ViewUnregistered( const AViewID inViewID ) {}//#893
	
	//印刷 #182
  public:
	void					NVI_PrintPageSetup( const AWindowID inWindowID );
	void					NVI_Print( const AWindowID inWindowID, const AText& inJobTitle );
  protected:
	CPrintImp&				NVM_GetPrintImp() { return mPrintImp; }
  private:
	CPrintImp							mPrintImp;
	
	//Object情報取得
  public:
	virtual AConstCharPtr	GetClassName() const { return "ADocument"; }
	
};

typedef AAbstractFactoryForObjectArray<ADocument>	ADocumentFactory;
