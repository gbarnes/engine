#pragma once

namespace Dawn
{
	class World;
	struct Material;

	void Editor_ReadGizmoManipulation(World* InWorld);
	void Editor_WriteGizmoManipulation(World* InWorld);
	void Editor_ShowMaterial(Material* InMaterial);
}