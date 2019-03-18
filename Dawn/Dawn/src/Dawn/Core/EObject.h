#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //
#include "RefPtr.h"
#include "inc_common.h"

namespace Dawn
{
	
	class DAWN_API EObject
	{
	public:
		//-------------------------------------------------------------------------
		// Constructor & Destructor
		//-------------------------------------------------------------------------
		EObject::EObject();
		virtual EObject::~EObject();

	public:
		//-------------------------------------------------------------------------
		// Public Methods
		//-------------------------------------------------------------------------
		void EObject::AddRef() const;
		virtual void EObject::Release() const;

	private:
		mutable unsigned int RefCount;
	};

	typedef RefPtr<EObject> EObjectPtr;
}