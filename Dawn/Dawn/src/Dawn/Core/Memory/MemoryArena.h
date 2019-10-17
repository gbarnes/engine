#pragma once

namespace Dawn
{
	struct SourceInfo
	{
		const char* Filename;
		u32 Line;

		SourceInfo(const char* InFileName, u32 InLine)
			: Filename(InFileName), Line(InLine)
		{
		}
	};

	namespace Memory
	{
		class NoBoundsChecking
		{
		public:
			static const size_t SIZE_FRONT = 0;
			static const size_t SIZE_BACK = 0;

			inline void GuardFront(void*) const {}
			inline void GuardBack(void*) const {}

			inline void CheckFront(const void*) const {}
			inline void CheckBack(const void*) const {}
		};

		class NoMemoryTagging
		{
		public:
			inline void TagAllocation(void*, u32) const {}
		};

		class SingleThread
		{
		public:
			inline void Enter(void) const {}
			inline void Leave(void) const {}
		};

		template <class SynchronizationPrimitive>
		class MultiThread
		{
		public:
			inline void Enter(void)
			{
				Primitive.Enter();
			}

			inline void Leave(void)
			{
				Primitive.Leave();
			}

		private:
			SynchronizationPrimitive Primitive;
		};

		class HeapArea
		{
		public:
			HeapArea(size_t InSize)
				: Size(InSize)
			{
				MemoryStart = static_cast<char*>(malloc(InSize));
				MemoryEnd = static_cast<char*>(MemoryStart) + InSize;
			}

			~HeapArea()
			{
				free((void*)MemoryStart);
			}

			void* GetStart() const
			{
				return MemoryStart;
			}

			void* GetEnd() const
			{
				return MemoryEnd;
			}

		private:
			char* MemoryEnd;
			char* MemoryStart;
			size_t Size;
		};
	}

	template<class AllocationPolicy, class ThreadPolicy, class BoundsPolicy, class MemoryTaggingPolicy>
	class MemoryArena
	{
	public:
		template <class AreaPolicy>
		explicit MemoryArena(const AreaPolicy& InArea)
		{
			Allocator = AllocationPolicy(InArea.GetStart(), InArea.GetEnd());
		}

		void* Allocate(size_t InSize, size_t InAlignment, SourceInfo& InSourceInfo)
		{
			ThreadGuard.Enter();

			const size_t OriginalSize = InSize;
			const size_t NewSize = InSize + BoundsPolicy::SIZE_BACK;

			char* PlainMemory = static_cast<char*>(Allocator.Allocate(NewSize, InAlignment, BoundsPolicy::SIZE_FRONT));

			BoundChecker.GuardFront(PlainMemory);
			MemoryTagger.TagAllocation(PlainMemory + BoundsPolicy::SIZE_FRONT, OriginalSize);
			BoundChecker.GuardBack(PlainMemory + BoundsPolicy::SIZE_FRONT + OriginalSize);

			ThreadGuard.Leave();

			return (PlainMemory + BoundsPolicy::SIZE_FRONT);
		}

		void Free(void* InPtr)
		{
			ThreadGuard.Enter();

			char* OriginalMemory = static_cast<char*>(InPtr) - BoundsPolicy::SIZE_FRONT;
			const size_t AllocationSize = Allocator.GetAllocationSize(OriginalMemory);

			BoundChecker.CheckFront(OriginalMemory);
			BoundChecker.CheckBack(OriginalMemory + AllocationSize - BoundsPolicy::SIZE_BACK);

			MemoryTagger.TagDeallocation(OriginalMemory, AllocationSize);

			Allocator.Free(OriginalMemory);
			ThreadGuard.Leave();
		}

	private:
		MemoryTaggingPolicy MemoryTagger;
		AllocationPolicy Allocator;
		ThreadPolicy ThreadGuard;
		BoundsPolicy BoundChecker;
	};

	
}