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

CocoaApp

*/

#pragma once

#import <Cocoa/Cocoa.h>
#import "Functions.h"
#include "../AbsBase/ABase.h"
#include "../AbsBase/ADataBase.h"
#include "../AbsFrame/AbsFrame.h"
#import <Sparkle/SUUpdater.h>

/**
アプリケーション
*/
@interface CocoaApp : NSApplication
{
	IBOutlet	NSView*	openCustomView;//#1034
}
- (NSView*)getOpenCustomView;//#1034
+ (void)appleScriptNotHandledError;//#1034
+ (void)appleScriptNotHandledError_DocumentReadOnly;//#1034
+ (void)appleScriptNoSuchObjectError:(NSString*)errorString;//#1034
@end

