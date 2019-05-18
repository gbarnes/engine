#pragma once
#include "inc_common.h"
#include "inc_core.h"
#include "Component.h"
#include "Transform/Transform.h"

namespace Dawn
{
	typedef GenericHandle SceneNodeId;

	struct SceneNode
	{
		ComponentId TransformComponent;
		SceneNodeId Id;
		SceneNodeId Parent;
		std::vector<SceneNode> Children;
	};

#define MAX_NUM_SCENE_NODES 8096

	class SceneGraph
	{
	public:
		
		void AttachTransform(Transform* InTransform)
		{
			//InTransform->SceneId = 
		}

	private:
		std::vector<std::unique_ptr<SceneNode>> Nodes;
	};


}