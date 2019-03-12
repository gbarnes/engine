#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "Job.h"

namespace Dawn
{
	static const unsigned int NUMBER_OF_JOBS = 4096u;
	static const unsigned int MASK = NUMBER_OF_JOBS - 1u;

	class CJobQueue
	{
	public:
		CJobQueue::CJobQueue();
		void Push(SJob* InJob);
		SJob* Pop();
		SJob* Steal();

	private:
		SJob* Jobs[NUMBER_OF_JOBS];
		long Top;
		long Bottom;
	};
}