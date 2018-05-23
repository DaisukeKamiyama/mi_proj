// Copyright ｩ2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.

//This file is the various define values extracted from PowerPlant header files
//by daisuke for mi (http://www.mimikaki.net) 2009/2/15 from HERE

#pragma once

// ---------------------------------------------------------------------------
//	PowerPlant version number

#define		__PowerPlant__	0x02228000	// Version 2.2.2


// ---------------------------------------------------------------------------
//	Target OS

#define PP_Target_Classic		0
#define PP_Target_Carbon		1

#define __MACH__	1


#define PP_Begin_Namespace_PowerPlant	namespace PowerPlant {
#define PP_End_Namespace_PowerPlant		}
#define PP_Using_Namespace_PowerPlant	using namespace PowerPlant;
#define PP_PowerPlant					PowerPlant

// ---------------------------------------------------------------------------
//	Types for PowerPlant Identifiers

typedef		SInt32			CommandT;
typedef		SInt32			MessageT;

typedef		SInt16			ResIDT;
typedef		SInt32			PaneIDT;
typedef		FourCharCode	ClassIDT;
typedef		FourCharCode	DataIDT;
typedef		FourCharCode	ObjectIDT;



const SInt8		max_Int8		= 127;
const SInt8		min_Int8		= -128;

const UInt8		max_Uint8		= 255;

const SInt16	max_Int16		= 32767;
const SInt16	min_Int16		= -32768;

const SInt32	max_Int32		= 0x7FFFFFFF;	//  2,147,483,647
const SInt32	min_Int32		= 0x80000000;	// -2,147,483,648

const ResIDT	resID_Default	= 32766;
const ResIDT	resID_Undefined	= 32767;

const SInt16	ScrollBar_Size	= 16;

const SInt16	Button_Off		= 0;
const SInt16	Button_On		= 1;
const SInt16	Button_Mixed	= 2;

const UInt32	delay_Feedback	= 8;	// Ticks to delay for visual feedback

					// ID for a Pane which does not exist
const PaneIDT	PaneIDT_Undefined	= -1;

					// ID for a Pane which exists, but has not been
					//   assigned a unique ID number
const PaneIDT	PaneIDT_Unspecified	= -2;

const PaneIDT	PaneIDT_HorizontalScrollBar	= -3;
const PaneIDT	PaneIDT_VerticalScrollBar	= -4;

					// Constants for inRefresh parameter for LPane functions
const bool		Refresh_Yes		= true;
const bool		Refresh_No		= false;

					// Constants for executing AppleEvents
const bool		ExecuteAE_Yes	= true;
const bool		ExecuteAE_No	= false;

					// Constants for recording AppleEvents
const bool		RecordAE_Yes	= true;
const bool		RecordAE_No		= false;

					// Constants for inThrowFail parameters
const bool		Throw_Yes		= true;
const bool		Throw_No		= false;

					// Pascal string with zero length byte
extern const unsigned char	Str_Empty[];

					// Pascal string with 1 character
extern const unsigned char	Str_Dummy[];

extern const Point		Point_00;
extern const Rect		Rect_0000;

const Size		Size_Zero		= 0;
const Handle	Handle_Nil		= 0;
const Ptr		Ptr_Nil			= 0;

#include "./PowerPlant/UException.h"

#define SignalIf_(test) 
#define StDisableDebugThrow_()  
#define SignalStringLiteral_(str) 

#define Assert_(test) 

