#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //
/*
typedef void(*BackendDispatchFunction)(const void*);

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Engine base class that provides reference counting. In order to handle 
// reference counting use AddRef and Release! This functions as the main class
// for engine specific objects such as gameobjects or others. 
//-----------------------------------------------------------------------------
template<typename T>
class TRenderCmdBucket
{
	typedef T Key;
public:
	template<typename U>
	U* AddCommand(Key InKey, BackendDispatchFunction InMethod);
	template<typename T>
	void Submit();
private:
	unsigned int CommandCount;
	Key* Keys;
	void** Data;
	BackendDispatchFunction* Functions;

	template<typename U>
	void Add(Key InKey, U* InData, BackendDispatchFunction InFunction);
};

template<typename U>
void TRenderCmdBucket::Add(Key InKey, U* InData, BackendDispatchFunction InFunction)
{
	Data[CommandCount - 1] = InData;
	Keys[CommandCount - 1] = InKey;
	Functions[CommandCount - 1] = InFunction;
}

template<typename T>
void TRenderCmdBucket::Submit()
{
	for (unsigned int i = 0; i < CommandCount; ++i)
	{
		Key key = Keys[i];
		void* data = Data[i];
	}
}

template<typename U>
U* TRenderCmdBucket::AddCommand(Key InKey, BackendDispatchFunction InMethod)
{
	U* Data = AllocateCommand<U>();

	Add(InKey, Data, InMethod);

	return Data;
}*/


