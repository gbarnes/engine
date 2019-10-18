#pragma once
#include "MemoryArena.h"
#include "MemoryAllocators.h"
// 
// Source Code "copied" and taken from reference points
// https://blog.molecular-matters.com/2011/07/07/memory-system-part-2/
// https://www.gamedev.net/articles/programming/general-and-gameplay-programming/c-custom-memory-allocation-r3010/
// http://bitsquid.blogspot.com/2010/09/custom-memory-allocation-in-c.html
//

#define D_NEW(Type, MemoryArena) new (MemoryArena.Allocate(sizeof(Type), __alignof(Type), SourceInfo(__FILE__, __LINE__))) Type
#define D_NEW_ALLOC(Type, Offset, Allocator) new (Allocator.Allocate(sizeof(Type), __alignof(Type), Offset)) Type
#define D_DELETE(Type, MemoryArena) Delete(Type, MemoryArena)
#define D_NEW_ARRAY(Type, MemoryArena) NewArray<TypeAndCount<Type>::Type>(MemoryArena, TypeAndCount<Type>::Count,  __FILE__, __LINE__, IntToType<IsPOD<TypeAndCount<type>::Type>::Value>())
#define D_DELETE_ARRAY(Object, MemoryArena) DeleteArray(Object, MemoryArena)


namespace Dawn
{
	template <typename T>
	struct IsPOD
	{
		static const bool Value = std::is_pod<T>::value;
	};

	template <bool I>
	struct IntToType
	{
	};

	typedef IntToType<false> NonPODType;
	typedef IntToType<true> PODType;

	template <class T>
	struct TypeAndCount
	{
	};

	template <class T, size_t N>
	struct TypeAndCount<T[N]>
	{
		typedef T Type;
		static const size_t Size = N;
	};


	namespace Memory
	{
		template <typename T, class ARENA>
		static void Delete(T* object, ARENA& arena)
		{
			// call the destructor first...
			object->~T();

			// ...and free the associated memory
			arena.Free(object);
		}

		template<typename T, class ARENA> 
		static T* NewArray(ARENA& InArena, size_t InNum, const char* InFilename, i32 InLine, NonPODType)
		{
			union
			{
				void* AsVoid;
				size_t* AsSize;
				T* AsT;
			};

			
			AsVoid = InArena.Allocate(sizeof(T) * N + sizeof(size_t), __alignof(T), SourceInfo() { __FILE__, __LINE__ });
			
			// we need to add one size_t in order to save the number of instances this array
			// contains (this is necessary for deleting the instances later on due to non-pod types having a constructor)
			*AsSize++ = InNum;

			// now we create all instances and look for the memory address that is one larger than our
			// allocated space.
			const T* const OverflowEnd = AsT + N;
			while (asT < OverflowEnd)
				new (asT++) T;

			// move back to the beginning of the array to return the first memory address.
			return (asT - N);
		}

		
		template<typename T, class ARENA>
		static void DeleteArray(T* InPtr, ARENA& InArena)
		{
			// since don't pass the type in the macro but have the type 
			// we just added a helper function which takes care of 
			// deducing the type to IsPOD statement.
			DeleteArray(InPtr, InArena, IntToType<IsPOD<T>::Value>());
		}

		template<typename T, class ARENA>
		static void DeleteArray(T* InPtr, ARENA& InArena, NonPODType)
		{
			union
			{
				size_t* AsSizeT;
				T* AsT;
			};
			
			// grab the instances by moving back the entire array - 1
			size_t NumInstances = AsSizeT[-1];

			// Iterate front to back and call constructors
			// for each element in the array!
			// We have to subtract one of the Index because of the 
			// first 4 bytes being reserved for the num of instances.
			for (size_t N = NumInstances; N > 0; --N)
				AsT[N - 1].~(); 

			InArena.Free(AsSizeT - 1);
		}

		template <typename T, class ARENA>
		T* NewArray(ARENA& InArena, size_t InNum, const char* InFile, int InLine, PODType)
		{
			return static_cast(InArena.Allocate(sizeof(T)*InNum, __alignof(T), { __FILE__, __LINE__ }));
		}

		template <typename T, class ARENA>
		void DeleteArray(T* InObject, ARENA& InArena, PODType)
		{
			// note --- for strict pod types we don't need to call a destructor
			InArena.Free(InObject);
		}
	}

	typedef MemoryArena<Allocators::LinearAllocator, Memory::SingleThread, Memory::NoBoundsChecking, Memory::NoMemoryTagging> SimpleArena;
}