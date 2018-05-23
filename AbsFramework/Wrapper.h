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

Wrapper

*/

#if IMPLEMENTATION_FOR_MACOSX

#include "MacWrapper/AMenuWrapper.h"
#include "MacWrapper/AColorWrapper.h"
#include "MacWrapper/ACursorWrapper.h"
#include "MacWrapper/AFileWrapper.h"
#include "MacWrapper/AKeyWrapper.h"
#include "MacWrapper/ALaunchWrapper.h"
#include "MacWrapper/AMouseWrapper.h"
#include "MacWrapper/ANetworkWrapper.h"
#include "MacWrapper/AScrapWrapper.h"
#include "MacWrapper/ASecurityWrapper.h"
#include "MacWrapper/ATimerWrapper.h"
#include "MacWrapper/AWindowWrapper.h"
#include "MacWrapper/ASoundWrapper.h"
#include "MacWrapper/ASpellCheckerWrapper.h"

extern ABool	WrapperComponentUnitTest();
#endif

#if IMPLEMENTATION_FOR_WINDOWS

#include "WinWrapper/AMenuWrapper.h"
#include "WinWrapper/AColorWrapper.h"
#include "WinWrapper/ACursorWrapper.h"
#include "WinWrapper/AFileWrapper.h"
#include "WinWrapper/AKeyWrapper.h"
#include "WinWrapper/ALaunchWrapper.h"
#include "WinWrapper/AMouseWrapper.h"
#include "WinWrapper/ANetworkWrapper.h"
#include "WinWrapper/AScrapWrapper.h"
#include "WinWrapper/ASecurityWrapper.h"
#include "WinWrapper/ATimerWrapper.h"
#include "WinWrapper/AWindowWrapper.h"
#include "WinWrapper/ASoundWrapper.h"

#endif


