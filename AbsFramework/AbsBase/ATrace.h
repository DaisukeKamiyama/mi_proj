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

ATrace

*/

#pragma once

#pragma mark ===========================
#pragma mark [名前空間]ATrace
/**トレースログを採取するための名前空間
ABaseのメンバ変数となる
*/
namespace ATrace
{
	void	Log( const AConstCharPtr inTraceType, const ABool inStderr, const AConstCharPtr inFilename, const ANumber inLineNumber, 
			const AConstCharPtr inString, const ABool inShowLogWindow, const AObjectArrayItem* inObjectArrayItem, const ABool inBacktrace );//B0000
	void	SetTraceAlwaysOutputToStderr();//For debug
	void	StartPostInternalEvent();//B0000
};

const AInternalEventType	kInternalEvent_ShowTraceLog								= 9999;
