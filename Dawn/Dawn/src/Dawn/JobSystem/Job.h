#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include <atomic>

namespace Dawn
{
	//-----------------------------------------------------------------------------
	// Typedefs
	//-----------------------------------------------------------------------------
	struct Job;
	typedef void(*JobFunction)(Job*);


	//-----------------------------------------------------------------------------
	// Main Struct Declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// 
	//-----------------------------------------------------------------------------
	struct Job
	{
		JobFunction Function;
		Job* Parent;
		std::atomic_uint UnfinishedJobs;
		char Data[52];
		std::atomic_uint ContinuationCount; // new
		Job* Continuations[15];    // new
	};
}