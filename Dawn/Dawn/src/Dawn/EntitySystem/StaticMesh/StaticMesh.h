#pragma once
#include "inc_common.h"
#include "inc_core.h"
#include "../../ResourceSystem/Resources.h"
#include "../../ResourceSystem/ResourceSystem.h"
#include "../Component.h"

namespace Dawn
{
	struct DAWN_API StaticMesh : public Component<StaticMesh>
	{
		REGISTER_TYPE(StaticMesh)

		static const u32 Version = 1;

		StaticMesh()
		{}

		MeshHandle MeshId;

		std::shared_ptr<Mesh> GetMesh()
		{
			return ResourceTable::GetMesh(MeshId);
		}
	};

	MAKE_TYPE_BEGIN(StaticMesh)
	MAKE_TYPE_END()
}