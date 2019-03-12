//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "EObject.h"
#include "inc_common.h"

namespace Dawn
{
	//-----------------------------------------------------------------------------
	// Creates a new object of type EObject. Sets the ref count to 0. 
	//-----------------------------------------------------------------------------
	CEObject::CEObject()
		: RefCount(0)
	{

	}

	//-----------------------------------------------------------------------------
	// Destructs the object and warns if the ref count isn't null!
	//-----------------------------------------------------------------------------
	CEObject::~CEObject()
	{
		//if(RefCount > 0)
			//SKY_PRINT_CONSOLE("Ref Count", "ref count of class to high!", 0x0f);
	}

	///////////////////////////////////////////////////////////////////////////////
	// CFile Member Definitions
	///////////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------------
	// Increses the reference count by one. Call this whenever you reference the 
	// object!
	//-----------------------------------------------------------------------------
	void CEObject::AddRef() const
	{
		++RefCount;
	}

	//-----------------------------------------------------------------------------
	// Decreases the reference count by one. Call this whenever you dereference the 
	// object! If the count falls to zero it will delete the object!
	//-----------------------------------------------------------------------------
	void CEObject::Release() const
	{
		//	assert(RefCount > 0);

		--RefCount;
		if (RefCount == 0)
		{
			delete ((CEObject *)this);
		}
	}
}

