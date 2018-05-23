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

Frame.h

*/

#pragma once

#include "AbsFrame/AApplication.h"
#include "AbsFrame/ADocument.h"
#include "AbsFrame/AWindow.h"
#include "AbsFrame/AThread.h"
#include "AbsFrame/AView.h"
#include "AbsFrame/AView_List.h"
#include "AbsFrame/AMenuManager.h"
#include "AbsFrame/ADynamicMenu.h"
#include "AbsFrame/InlineFunctions.h"
#include "AbsFrame/AView_EditBox.h"
#include "AbsFrame/ADocument_EditBox.h"
#include "AbsFrame/AEditBoxUndoer.h"
#include "AbsFrame/AView_Frame.h"
#include "AbsFrame/AView_Button.h"
#include "AbsFrame/ATextArrayMenuManager.h"
#include "AbsFrame/AView_VSeparator.h"
#include "AbsFrame/AView_HSeparator.h"
#include "AbsFrame/AWindow_VSeparatorOverlay.h"
#include "AbsFrame/AWindow_HSeparatorOverlay.h"
#include "AbsFrame/AWindow_ButtonOverlay.h"
#include "AbsFrame/AWindow_OK.h"
#include "AbsFrame/AWindow_OKCancel.h"
#include "AbsFrame/AWindow_OKCancelDiscard.h"
#include "AbsFrame/AWindow_SaveCancelDiscard.h"
#include "AbsFrame/AWindow_ModalAlert.h"
#include "AbsFrame/AWindow_ModalCancelAlert.h"
#include "AbsFrame/ACurlThread.h"
#include "AbsFrame/AView_Plain.h"
#include "AbsFrame/AView_SizeBox.h"
#include "AbsFrame/AView_TextPanel.h"
#include "AbsFrame/AWindow_ToolTip.h"

extern ABool	FrameComponentUnitTest();

