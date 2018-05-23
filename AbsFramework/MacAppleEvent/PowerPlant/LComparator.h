// Copyright ©2005, 2006 Freescale Semiconductor, Inc.
// Please see the License for the specific language governing rights and
// limitations under the License.
// ===========================================================================
//	LComparator.h				PowerPlant 2.2.2	©1995-2005 Metrowerks Inc.
// ===========================================================================
//
//	LComparator
//		Compares two items of arbitrary length
//
//	LLongComparator
//		Compares two items which are sizeof(long)

#ifndef _H_LComparator
#define _H_LComparator
#pragma once

//Modified by daisuke for mi (http://www.mimikaki.net) 2009/2/15 from HERE
//Comment-out the following 1 line
//#include <PP_Prefix.h>
//Added the following 1 line
#include "MyPPHeader.h"
//Modified by daisuke for mi (http://www.mimikaki.net) 2009/2/15 to HERE

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

PP_Begin_Namespace_PowerPlant

typedef const void* CompareKeyT;

// ===========================================================================
//	¥ LComparator ¥
// ===========================================================================

class	LComparator {
public:
						LComparator();
						
	virtual				~LComparator();

	virtual SInt32		Compare(
								const void*			inItemOne,
								const void* 		inItemTwo,
								UInt32				inSizeOne,
								UInt32				inSizeTwo) const;

	virtual Boolean		IsEqualTo(
								const void*			inItemOne,
								const void* 		inItemTwo,
								UInt32				inSizeOne,
								UInt32				inSizeTwo) const;

	virtual	SInt32		CompareToKey(
								const void*			inItem,
								UInt32				inSize,
								const void*			inKey) const;

	virtual	Boolean		IsEqualToKey(
								const void*			inItem,
								UInt32				inSize,
								const void*			inKey) const;

	virtual LComparator*	Clone();

	static LComparator*	GetComparator();

protected:
	static	LComparator*	sComparator;
};


// ===========================================================================
//	¥ LLongComparator ¥
// ===========================================================================

class	LLongComparator : public LComparator {
public:
						LLongComparator();
						
	virtual				~LLongComparator();

	virtual SInt32		Compare(
								const void*			inItemOne,
								const void* 		inItemTwo,
								UInt32				inSizeOne,
								UInt32				inSizeTwo) const;

	virtual Boolean		IsEqualTo(
								const void*			inItemOne,
								const void* 		inItemTwo,
								UInt32				inSizeOne,
								UInt32				inSizeTwo) const;

	virtual LComparator*	Clone();

	static LLongComparator*		GetComparator();

protected:
	static LLongComparator*	sLongComparator;
};

PP_End_Namespace_PowerPlant


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
