#pragma once
#include "Resources.h"
#include "inc_common.h"

namespace Dawn
{
	class DAWN_API ResourceTable
	{
	public:
		static u128 ResourceCount(const ResourceType InType);
		static bool TrackResource(ResourceType InType, void* InResource);
		static GenericHandle LookUpResource(ResourceType InType, FileHandle InResource);

		// specialized methods to receive resources of specific types.

		static std::shared_ptr<Mesh> GetMesh(MeshHandle InHandle);
		static std::shared_ptr<Shader> GetShader(const ShaderHandle& InHandle);
		static std::shared_ptr<Image> GetImage(ImageHandle InHandle);
		static void Shutdown();
	private:
		static std::vector<std::shared_ptr<Mesh>> Meshes;
		static std::vector<std::shared_ptr<Material>> Materials;
		static std::vector<std::shared_ptr<Shader>> Shaders;
		static std::vector<std::shared_ptr<Image>> Images;
	};
}