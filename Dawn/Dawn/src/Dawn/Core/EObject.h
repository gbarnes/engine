#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //
#include "RefPtr.h"
#include "inc_common.h"

namespace Dawn
{
	
	class DAWN_API CEObject
	{
	public:
		//-------------------------------------------------------------------------
		// Constructor & Destructor
		//-------------------------------------------------------------------------
		CEObject::CEObject();
		virtual CEObject::~CEObject();

	public:
		//-------------------------------------------------------------------------
		// Public Methods
		//-------------------------------------------------------------------------
		void CEObject::AddRef() const;
		virtual void CEObject::Release() const;

	private:
		mutable unsigned int RefCount;
	};

	typedef CRefPtr<CEObject> CEObjectPtr;
}