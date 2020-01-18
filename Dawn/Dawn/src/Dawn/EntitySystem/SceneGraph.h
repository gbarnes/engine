#pragma once
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
		std::vector<SceneNodeId> Children;
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