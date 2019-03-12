#pragma once

namespace Dawn
{
	// Template taken from http://www.codeproject.com/Articles/64111/Building-a-Quick-and-Handy-Reference-Counting-Clas
	template < class Type >
	class CRefPtr
	{
	public:
		CRefPtr(Type* InObject = NULL)
			: Object(InObject)
		{
			if (Object != NULL)
				Object->AddRef();
		};

		CRefPtr(const CRefPtr &InObject)
			: Object(InObject.m_pObject)
		{
			if (Object != NULL)
				Object->AddRef();
		};

		~CRefPtr()
		{
			if (Object != NULL)
				Object->Release();
		};

	public:
		//Assign a pointer
		//e.g. x = new T();
		CRefPtr &operator=(Type* InObject)
		{
			//The following grab and release operations have to be performed
			//in that order to handle the case where ptr == mPtr
			//(See comment below by David Garlisch)
			if (InObject != NULL)
				InObject->AddRef();

			if (Object != NULL)
				Object->Release();

			Object = InObject;

			return (*this);
		}

		//Assign another RCPtr
		CRefPtr &operator= (const CRefPtr &InObject)
		{
			return (*this) = InObject.Object;
		}

		//Retrieve actual pointer
		Type* Get() const
		{
			return Object;
		}

	public:
		//Some overloaded operators to facilitate dealing with an RCPtr 
		//as a conventional C pointer.
		//Without these operators, one can still use the less transparent 
		//get() method to access the pointer.

		Type* operator->() const { return Object; }
		Type &operator*() const { return *Object; }
		operator Type*() const { return Object; }
		operator bool() const { return Object != NULL; }
		bool operator==(const CRefPtr &InObject) { return Object == InObject.Object; }
		bool operator==(const Type* InObject) { return Object == InObject; }

	private:
		Type* Object;
	};

	
}