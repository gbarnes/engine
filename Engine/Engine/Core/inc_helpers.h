#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //
#include "../inc_common.h"



//-----------------------------------------------------------------------------
// Explodes a given string into pieces by separator. Stores the results in a 
// vector. Found at:
// http://www.infernodevelopment.com/perfect-c-string-explode-split
//-----------------------------------------------------------------------------
void StringExplode(CString InString, CString InSeparator, std::vector<CString>* OutResults);

/*
	Taken from http://blog.molecular-matters.com/2013/05/17/adventures-in-data-oriented-design-part-3b-internal-references/
*/
template <size_t N1, size_t N2>
struct SGenericHandle
{
	u32 Index : N1;
	u32 Generation : N2;
};