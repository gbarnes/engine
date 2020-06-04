#pragma once

namespace Dawn
{
#define ARRAY_GROWTH_FACTOR 4
#define ARRAY_INITIAL_SIZE 4

	template < typename T >
	class Array 
	{
	private:
		T* Values;
		u64 Size;
		u64 Capacity;

	public:
		Array() 
		{
			this->Size = 0;
			this->Capacity = ARRAY_INITIAL_SIZE;
			T* TempData = (T*)malloc(sizeof(T) * Capacity);
			assert(TempData);
			this->Values = TempData;
		}

		Array(const Array &InArrayToCopy) 
			: Size(InArrayToCopy.Size), this->Capacity(InArrayToCopy.Capacity)
		{
			T* TempData = (T*)malloc(sizeof(T) * InArrayToCopy.Capacity);
			assert(TempData);
			this->Values = TempData;

			memcpy(this->Values, InArrayToCopy.Values, sizeof(T) * Capacity);
		}

		~Array() 
		{
			if (this->Values)
			{
				free(this->Values);
				this->Values = nullptr;
			}
		}

		void Clear()
		{
			memset(this->Values, 0, sizeof(T) * Capacity);
			this->Size = 0;
		}

		void Push(T Data) 
		{
			if (this->Size >= this->Capacity)
				this->Resize();

			*(this->Values + this->Size++) = Data;
		}

		const T* Data() const
		{
			return &this->Values[0];
		}

		T* Data()
		{
			return &this->Values[0];
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

			// save the value thus we can return it later on!
			T TempValue = *(this->Values + Index);

			// otherwise we will have to allocate a new block of memory and copy 
			// the elements over from the other one!
			T* NewMemoryBlock = (T*)malloc(sizeof(T) * Capacity);

			// Now lets copy over the first half of the memory From Index 0 to Index - 1
			memcpy(NewMemoryBlock, this->Values, sizeof(T) * (Index));

			// Afterwards we need to set the pointer to the Index+1 for the old data 
			// and calculate the remaining bytes we want to copy over to the new 
			// position in the array.
			memcpy((NewMemoryBlock + Index), (this->Values + (Index+1)), sizeof(T) * (Size - (Index + 1)));
			
			this->Size--;

			// free the old memory and set the temp allocated one!
			free(this->Values);
			this->Values = NewMemoryBlock;
			return TempValue;
		}


		i64 Count() const
		{
			return this->Size;
		}

	public:
		T operator[](i32 Index) const
		{
			assert(Index < this->Size);
			return this->Values[Index];
		}

		T& operator[](i32 Index)
		{
			assert(Index < this->Size);
			return this->Values[Index];
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
