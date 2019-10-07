#include "ResourceTable.h"
#include "Brofiler.h"

namespace Dawn
{
	std::vector<Shared<Model>> ResourceTable::Models;
	std::vector<Shared<Mesh>> ResourceTable::Meshes;
	std::vector<Shared<Material>> ResourceTable::Materials;
	std::vector<Shared<Shader>> ResourceTable::Shaders;
	std::vector<Shared<Image>> ResourceTable::Images;

	Shared<Model> ResourceTable::GetModel(ModelHandle InHandle)
	{
		if (Models.size() == 0)
			return std::make_shared<Model>();

		u32 index = InHandle.Index;
		if (Models.size() < index)
			return nullptr;

		return Models[index];
	}


	Shared<Mesh> ResourceTable::GetMesh(MeshHandle InHandle)
	{
		if (Meshes.size() == 0)
			return std::make_shared<Mesh>();

		u32 index = InHandle.Index;
		if (Meshes.size() < index)
			return nullptr;

		return Meshes[index];
	}

	Shared<Shader> ResourceTable::GetShader(const ShaderHandle& InHandle)
	{
		if (Shaders.size() == 0)
			return std::make_shared<Shader>();

		u32 index = InHandle.Index;
		if (Shaders.size() < index)
			return nullptr;

		return Shaders[index];
	}

	Shared<Image> ResourceTable::GetImage(ImageHandle InHandle)
	{
		if (Images.size() == 0)
			return std::make_shared<Image>();

		u32 index = InHandle.Index;
		if (Images.size() < index)
			return nullptr;

		return Images[index];
	}

	void ResourceTable::Shutdown()
	{

		Meshes.clear();
		Models.clear();
		Images.clear();
		Meshes.clear();
		Shaders.clear();
	}

	bool ResourceTable::TrackResource(ResourceType InType, void* InResource)
	{
		if (InType == ResourceType_Model) {
			auto modelPtr = Shared<Model>(static_cast<Model*>(InResource));
			Models.emplace_back(modelPtr);
			return true;
		}
		
		if (InType == ResourceType_StaticMesh) {
			auto meshPtr = Shared<Mesh>(static_cast<Mesh*>(InResource));
			Meshes.emplace_back(meshPtr);
			return true;
		}

		if (InType == ResourceType_Shader) {
			auto shaderPtr = Shared<Shader>(static_cast<Shader*>(InResource));
			Shaders.emplace_back(shaderPtr);
			return true;
		}

		if (InType == ResourceType_Image) {
			auto imagePtr = Shared<Image>(static_cast<Image*>(InResource));
			Images.emplace_back(imagePtr);
			return true;
		}

		return false;
	}

	GenericHandle ResourceTable::LookUpResource(ResourceType InType, FileHandle InResource)
	{
		BROFILE
		GenericHandle Id = INVALID_HANDLE;

		// This is a pretty bad solution for something like this and I don't like it
		// at all lets try to find a simpler solution later on, Gavin Barnes, 03/22/19
		if (InType == ResourceType_Model) {

			auto begin = Models.begin();
			auto end = Models.end();

			auto it = std::find_if(begin, end,
				[InResource](Shared<Model> n) -> bool {return n->FileId == InResource; });

			if (it != end)
				Id = it->get()->Id;
		}

		if (InType == ResourceType_Shader) {

			auto begin = Shaders.begin();
			auto end = Shaders.end();

			auto it = std::find_if(begin, end,
				[InResource](Shared<Shader> n) -> bool {return n->FileId == InResource; });

			if (it != end)
				Id = it->get()->Id;
		}

		if (InType == ResourceType_Image) {
			auto begin = Images.begin();
			auto end = Images.end();

			auto it = std::find_if(begin, end,
				[InResource](Shared<Image> n) -> bool {return n->FileId == InResource; });

			if (it != end)
				Id = it->get()->Id;
		}

		return Id;
	}

	u128 ResourceTable::ResourceCount(const ResourceType InType)
	{
		if (InType == ResourceType_Model)
			return Models.size();
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