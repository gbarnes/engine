#pragma once

namespace Dawn
{
	typedef GenericHandle EntityId;
	struct Transform;
	class World;

	struct EntityMetaData
	{
		UUID Guid;
		std::string Name;
		bool bIsHiddenInEditorHierarchy = false;
		bool PersistsThroughLoad = false;
		bool bIsActive = true;
		World* World = nullptr;
	};

	struct DAWN_API Entity
	{
		i32 Id = -1;
		u32 Generation = 0;
		
		FORCEINLINE bool IsValid()
		{
			return Id > -1;
		}

		Transform* GetTransform(World* InWorld);
	};

	#define INVALID_ENTITY {-1, 0}
}