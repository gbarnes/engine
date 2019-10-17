#include "MemoryAllocators.h"
#include "inc_common.h"

namespace Dawn
{

	bool IsPowerOfTwo(uintptr_t x) {
		return (x & (x - 1)) == 0;
	}

	inline uintptr_t AlignForward(uintptr_t InAddress, size_t InAlignment)
	{
		// note --- figure this out on paper?!
		// assume we have a alignment of 4 bytes and a group of memory
		// addresses: 
		// 0xXXXXXX04 <--- we start at this one and saved something thats 7 Bytes long
		// 0xXXXXXX0A <--- so the current next memory location is 0A. Now we want to save something with an alignment of 4 bytes. Instead of starting at 0A
		// 0xXXXXXX0C <--- we want to start here 
		// 0xXXXXXX10

		//
		// How does this work? First we compare the 
		// memory address with the bitmask of the alignment we
		// want to check against. In case the address is aligned 
		// this will return 0 since 0xFFFFFFF04 & 0x00000003
		//
		// 1111 1111 1111 1111 1111 1111 1111 0000 0100
		//					AND
		// 0000 0000 0000 0000 0000 0000 0000 0000 0011
		//					=
		// 0000 0000 0000 0000 0000 0000 0000 0000 0000
		//
		// In case the address is aligned we can just return 
		// it, otherwise we have to finally align it. 
		// we can do this by calculating the delta between 
		// the alignment and how many bytes we are off till 
		// it might be aligned and move the pointer by these
		// bytes forward. 
		// 

		uintptr_t NewPtr = InAddress;
		uintptr_t Comparison = (InAddress & (InAlignment - 1));

		if (Comparison != 0) 
		{
			NewPtr += InAlignment - Comparison;
		}

		return NewPtr;
	}

	Allocators::LinearAllocator::LinearAllocator(size_t size)
	{
		//VirtualAlloc()
		Reset();
	}

	Allocators::LinearAllocator::LinearAllocator(void* InStart, void* InEnd)
	{
		this->Start = static_cast<char*>(InStart);
		this->End = static_cast<char*>(InEnd);

		Reset();
	}

	void* Allocators::LinearAllocator::Allocate(size_t InSize, size_t InAlignment, size_t InOffset)
	{
		// offset pointer first, align it, and offset it back
		Current = (char*)AlignForward(((uintptr_t)Current + InOffset), InAlignment) - InOffset;

		void* UserPtr = Current;
		Current += InSize;

		if (Current >= End)
		{
			// out of memory
			return nullptr;
		}

		return UserPtr;
	}

}