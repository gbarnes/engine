#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

#include <Windows.h>
#include <thread>

namespace Dawn {
	//-----------------------------------------------------------------------------
	// Main Interface Declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// Basic interface that provides necessary implementation details for       
	// any platform view within the project. Basically it is used to          
	// create windows for linux, windows or mac os which we can render to
	// at specific point.
	//-----------------------------------------------------------------------------
	template<class T, class P>
	class TThread
	{

	public:
		//-----------------------------------------------------------------------------
		// Typedefinitions
		//-----------------------------------------------------------------------------
		typedef void (T::*ThreadFunc)(P);

		//-----------------------------------------------------------------------------
		// Creates a new instance of the windows 32 thread implementation.
		//-----------------------------------------------------------------------------
		TThread() :
			Instance(nullptr),
			Function(nullptr)
		{
			this->Thread = nullptr;
			this->ThreadID = (DWORD)-1;
		}

		//-----------------------------------------------------------------------------
		// Destructs the object if the object is still active it can't bet terminated
		//-----------------------------------------------------------------------------
		virtual ~TThread()
		{
			if (this->IsActive())
				this->Terminate();
		}

		//-------------------------------------------------------------------------
		// Public Methods
		//-------------------------------------------------------------------------
		bool TThread::Run(T* InClass, ThreadFunc InFunction, P InParams);
		void TThread::Suspend(void);
		void TThread::Resume(void);
		void TThread::Terminate(void);
		bool TThread::IsActive(void) const;

	protected:
		//-------------------------------------------------------------------------
		// Private Members
		//-------------------------------------------------------------------------
		HANDLE Thread;
		DWORD ThreadID;
		T* Instance;
		ThreadFunc Function;
		P Param;

		//-------------------------------------------------------------------------
		// Static Private Members
		//-------------------------------------------------------------------------
		static DWORD WINAPI StartThread(void* pParam);
	};

	//-----------------------------------------------------------------------------
	// Starts the thread with the supplied threading function. 
	//-----------------------------------------------------------------------------
	template<class T, class P>
	bool TThread<T, P>::Run(T* InClass, ThreadFunc InFunction, P InParams)
	{
		this->Instance = InClass;
		this->Function = InFunction;
		this->Param = InParams;

		this->Thread = CreateThread(NULL, 0, TThread<T, P>::StartThread, this, 0, &ThreadID);

		return (this->Thread != NULL);
	}

	//-----------------------------------------------------------------------------
	// Suspends the given thread.
	//-----------------------------------------------------------------------------
	template<class T, class P>
	void TThread<T, P>::Suspend(void)
	{
		if (this->Thread)
			::SuspendThread(this->Thread);
	}

	//-----------------------------------------------------------------------------
	// Resumes the thread again.
	//-----------------------------------------------------------------------------
	template<class T, class P>
	void TThread<T, P>::Resume(void)
	{
		if (this->Thread)
		{
			int resumeCount = ::ResumeThread(this->Thread);
			while (resumeCount > 1)
				resumeCount = ::ResumeThread(this->Thread);
		}
	}

	//-----------------------------------------------------------------------------
	// Terminates the thread and will close the handle.
	//-----------------------------------------------------------------------------
	template<class T, class P>
	void TThread<T, P>::Terminate(void)
	{
		if (this->Thread)
		{
			::TerminateThread(this->Thread, 0);
			::CloseHandle(this->Thread);
			this->Thread = NULL;
		}
	}

	//-----------------------------------------------------------------------------
	// Checks whether or not the thread is still active or not.
	//-----------------------------------------------------------------------------
	template<class T, class P>
	bool TThread<T, P>::IsActive(void) const
	{
		return ((this->Thread != NULL) && (::WaitForSingleObject(this->Thread, 0) != WAIT_OBJECT_0));
	}

	//-----------------------------------------------------------------------------
	// Static helper function in order to succesffully start the supplied class 
	// method on the instance.
	//-----------------------------------------------------------------------------
	template<class T, class P>
	DWORD WINAPI TThread<T, P>::StartThread(void* InParams)
	{
		TThread* Instance = reinterpret_cast<TThread*> (InParams);

		if (!Instance)
			return (DWORD)-1;

		T* ClassInstance = Instance->Instance;
		ThreadFunc Func = Instance->Function;
		P param = Instance->Param;

		((*ClassInstance).*Func)(param);

		return 0;
	}
}