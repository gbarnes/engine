#pragma once
#include "inc_common.h"

namespace Dawn
{
	template<typename T>
	class HandleObjectArray;

	template<typename T>
	class DAWN_API HandleObjectSlot
	{
	public:
		T* Element;

		GenericHandle GetId() const
		{
			return Id;
		}

		bool IsValid()
		{
			return Element != nullptr;
		}

	private:
		GenericHandle Id;

		friend class HandleObjectArray<T>;
	};

	// 
	// An array that contains handles mapped to specific
	// memory content. Handles will be reused once a slot 
	// is freed!
	// 
	template<typename T>
	class DAWN_API HandleObjectArray
	{
	public:
		HandleObjectArray()
		{
			FreeHandle.IsValid = true;
			FreeHandle.Generation = 0;
			FreeHandle.Index = 0;
		}

		HandleObjectSlot<T>* Request()
		{
			if (Slots.size() > FreeHandle.Index)
			{
				auto& Slot = Slots[FreeHandle.Index];
				if (!Slot.IsValid())
				{
					Slot.Id.Generation++;
					return &Slot;
				}
			}

			HandleObjectSlot<T> Slot;
			Slot.Id.Index = (u32)(Slots.size() - 1);
			Slot.Id.Generation = 1;
			Slot.Id.IsValid = true;

			FreeHandle.Index = ++Slot.Id.Index;
			FreeHandle.Generation = 0;

			return &Slots.emplace_back(Slot);
		}

		T* Find(const GenericHandle& Id)
		{
			assert(Id.IsValid == true);
			auto& Element = Slots[Id.Index];
			assert(Element.Id.Generation == Id.Generation);
			return Slots[Id.Index].Element;
		}

		void Free(const GenericHandle& Id)
		{
			if (!Id.IsValid)
				return;

			FreeHandle = Id;
			auto& Slot = Slots[Id.Index];

			delete Slot.Element;
			Slot.Element = nullptr;
		}

		void Clear()
		{
			for (auto Slot : Slots)
			{
				if (Slot.Element != nullptr)
				{
					delete Slot.Element;
					Slot.Element = nullptr;
				}
			}

			Slots.clear();

			FreeHandle.IsValid = true;
			FreeHandle.Generation = 0;
			FreeHandle.Index = 0;
		}

	private:
		GenericHandle FreeHandle;
		std::vector<HandleObjectSlot<T>> Slots;
	};
}