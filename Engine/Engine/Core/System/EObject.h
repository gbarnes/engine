#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //
#include "ComPtr.h"

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Engine base class that provides reference counting. In order to handle 
// reference counting use AddRef and Release! This functions as the main class
// for engine specific objects such as gameobjects or others. 
//-----------------------------------------------------------------------------
class CEObject
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

typedef CComPtr<CEObject> CEObjectPtr;