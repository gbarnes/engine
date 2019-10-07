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
		static Shared<Model> GetModel(ModelHandle InHandle);
		static Shared<Mesh> GetMesh(MeshHandle InHandle);
		static Shared<Shader> GetShader(const ShaderHandle& InHandle);
		static Shared<Image> GetImage(ImageHandle InHandle);
		static void Shutdown();
	private:
		//static std::map<ResourceType, ResourceStorage<Model>>
		static std::vector<Shared<Model>> Models;
		static std::vector<Shared<Mesh>> Meshes;
		static std::vector<Shared<Material>> Materials;
		static std::vector<Shared<Shader>> Shaders;
		static std::vector<Shared<Image>> Images;
	};
}