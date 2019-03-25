#include "ResourceTable.h"
#include "Brofiler.h"

namespace Dawn
{
	std::vector<std::shared_ptr<Mesh>> ResourceTable::Meshes;
	std::vector<std::shared_ptr<Material>> ResourceTable::Materials;
	std::vector<std::shared_ptr<Shader>> ResourceTable::Shaders;
	std::vector<std::shared_ptr<Image>> ResourceTable::Images;

	std::shared_ptr<Mesh> ResourceTable::GetMesh(MeshHandle InHandle)
	{
		u32 index = InHandle.Index;
		if (Meshes.size() < index)
			return nullptr;

		return Meshes[index];
	}

	std::shared_ptr<Shader> ResourceTable::GetShader(ShaderHandle InHandle)
	{
		u32 index = InHandle.Index;
		if (Shaders.size() < index)
			return nullptr;

		return Shaders[index];
	}

	std::shared_ptr<Image> ResourceTable::GetImage(ImageHandle InHandle)
	{
		u32 index = InHandle.Index;
		if (Images.size() < index)
			return nullptr;

		return Images[index];
	}

	void ResourceTable::Shutdown()
	{
		Meshes.clear();
		Images.clear();
		Meshes.clear();
		Shaders.clear();
	}

	bool ResourceTable::TrackResource(ResourceType InType, void* InResource)
	{
		
		if (InType == ResourceType_StaticMesh) {
			auto meshPtr = std::shared_ptr<Mesh>(static_cast<Mesh*>(InResource));
			Meshes.emplace_back(meshPtr);
			return true;
		}

		if (InType == ResourceType_Shader) {
			auto shaderPtr = std::shared_ptr<Shader>(static_cast<Shader*>(InResource));
			Shaders.emplace_back(shaderPtr);
			return true;
		}

		if (InType == ResourceType_Image) {
			auto imagePtr = std::shared_ptr<Image>(static_cast<Image*>(InResource));
			Images.emplace_back(imagePtr);
			return true;
		}

		return false;
	}

	GenericHandle ResourceTable::LookUpResource(ResourceType InType, FileHandle InResource)
	{
		BROFILE
		GenericHandle Id = {};

		// This is a pretty bad solution for something like this and I don't like it
		// at all lets try to find a simpler solution later on, Gavin Barnes, 03/22/19
		if (InType == ResourceType_StaticMesh) {

			auto begin = Meshes.begin();
			auto end = Meshes.end();

			auto it = std::find_if(begin, end,
				[InResource](std::shared_ptr<Mesh> n) -> bool {return n->FileId == InResource; });

			if (it != end)
				Id = it->get()->Id;
		}

		if (InType == ResourceType_Shader) {

			auto begin = Shaders.begin();
			auto end = Shaders.end();

			auto it = std::find_if(begin, end,
				[InResource](std::shared_ptr<Shader> n) -> bool {return n->FileId == InResource; });

			if (it != end)
				Id = it->get()->Id;
		}

		if (InType == ResourceType_Image) {
			auto begin = Images.begin();
			auto end = Images.end();

			auto it = std::find_if(begin, end,
				[InResource](std::shared_ptr<Image> n) -> bool {return n->FileId == InResource; });

			if (it != end)
				Id = it->get()->Id;
		}

		return Id;
	}

	u128 ResourceTable::ResourceCount(const ResourceType InType)
	{
		if (InType == ResourceType_StaticMesh)
			return Meshes.size();
		if (InType == ResourceType_Image)
			return Images.size();
		if (InType == ResourceType_Material)
			return Materials.size();
		if (InType == ResourceType_Shader)
			return Shaders.size();

		return 0;
	}
}