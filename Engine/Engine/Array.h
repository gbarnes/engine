#pragma once
#include "inc_common.h"

namespace DS
{
#define ARRAY_GROWTH_FACTOR 4
#define ARRAY_INITIAL_SIZE 4

	template < typename T >
	class CArray 
	{
	private:
		T* Values;
		i64 Size;
		i64 Capacity;

	public:
		CArray() 
		{
			this->Size = 0;
			this->Capacity = ARRAY_INITIAL_SIZE;
			T* TempData = (T*)malloc(sizeof(T) * Capacity);
			assert(TempData);
			this->Values = TempData;
		}

		~CArray() 
		{
			free(this->Values);
		}

		void Push(T Data) 
		{
			if (this->Size >= this->Capacity)
				this->Resize();

			*(this->Values + this->Size++) = Data;
		}

		T Pop()
		{
			return *(this->Values + this->Size--);
		}

		T Remove(u32 Index)
		{
			// if the index we are deleting the last 
			// element in the array it makes no sense
			// to split the memory instead we just pop 
			// the element!
			if (Index == this->Size)
			{
				return Pop();
			}

			// otherwise we will have to allocate a new block of memory and copy 
			// the elements over from the other one!
			T* NewMemoryBlock = (T*)malloc(sizeof(this->Values) * Capacity);
			*(NewMemoryBlock) = *this->Values;
			*(NewMemoryBlock + (Index)) = *(this->Values + (Index + 1));
			this->Size--;

			// free the old memory and set the temp allocated one!
			free(this->Values);
			this->Values = NewMemoryBlock;
		}


		i64 Count() const
		{
			return this->Size;
		}

	public:
		T operator[](u32 Index)
		{
			assert(Index < this->Size);
			return *(this->Values + Index);
		}

	protected:
		// 
		void Resize() 
		{
			i64 newCapacity = Capacity * ARRAY_GROWTH_FACTOR;
			T* TempData = (T*)realloc(this->Values, sizeof(this->Values) * newCapacity);
			assert(TempData);
			this->Values = TempData;
			this->Capacity = newCapacity;
		}
	};

}
