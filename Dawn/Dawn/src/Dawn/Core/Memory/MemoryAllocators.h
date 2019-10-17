#pragma once


namespace Dawn
{
	namespace Allocators
	{
		class LinearAllocator
		{
		public:
			explicit LinearAllocator(size_t size);
			LinearAllocator() {}
			LinearAllocator(void* InStart, void* InEnd);

			void* Allocate(size_t InSize, size_t InAlignment, size_t InOffset);
			inline void Free(void* InPtr);
			
			inline void Reset()
			{
				Current = Start;
			}

		private:
			char* Start;
			char* End;
			char* Current;
		};
	}

}