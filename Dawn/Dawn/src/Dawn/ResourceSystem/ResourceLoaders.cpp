#include "ResourceLoaders.h"
#include "ResourceSystem.h"
#include "ResourceUtils.h"
#include "Core/GDI/GfxBackend.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Dawn
{
	//
	// Loads a simple static mesh into the resource system by using 
	// the tiny obj loader!
	// 
	MeshHandle RS_LoadStaticMesh(ResourceSystem* InFS, std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		// We might need to have another version that is used when loading binary
		// meshes when being in dist mode - Gavin Barnes, 03/22/19

		MeshHandle Id = ResourceTable::LookUpResource(InMetaData->Type, InMetaData->Id);
		if (Id.IsValid)
			return Id;

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;

		std::string combinedPath = InWorkspacePath + InMetaData->Path ;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, (combinedPath + InMetaData->Name).c_str(), InMetaData->Path.c_str(), true);
		if (ret)
		{              
			DWN_CORE_WARN(warn);

			// Create new handle or update existing one
			u32 meshIndex = (u32)ResourceTable::ResourceCount(ResourceType_StaticMesh);
			Id.Index = meshIndex;
			Id.IsValid = true;
			
			// The generation can always be 0 in this case since we don't allow 
			// to remove meshes while runtime at this time! - Gavin Barnes 03/22/19
			Id.Generation = 0;

			Mesh* mesh = new Mesh();
			mesh->Id = Id;
			mesh->FileId = InMetaData->Id;
			
			// load vertices and save to the mesh!
			{			
				// divide by three because we have three vertices per triangle!
				// we assume that the mesh is being triangulated which we told tinyobj when loading!
				u128 numOfVertices = attrib.vertices.size() / 3;

				for (u32 i = 0; i < numOfVertices; ++i)
				{
					//VertexPosNormalUV vertex = {};
					//VertexPosUV vertex = {};
					VertexPosColor vertex = {};
					vertex.Position = DirectX::XMFLOAT3(attrib.vertices[3 * i], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]);

					//if(attrib.normals.size() > 0)
					//	vertex.Normal = DirectX::XMFLOAT3(attrib.normals[3 * i], attrib.normals[3 * i + 1], attrib.normals[3 * i + 2]);

					float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					vertex.Color = DirectX::XMFLOAT3(r, r, r);

					mesh->Vertices.emplace_back(vertex);
				}
			}

			// load indices and submesh groups!
			{
				for (auto shape : shapes)
				{
					Submesh submesh = {};
					submesh.Name = shape.name;
					submesh.NumOfIndices = shape.mesh.indices.size();
					submesh.StartIndex = (u32)std::max((i32)mesh->Indices.size() - 1, 0);

					for (auto index : shape.mesh.indices)
					{
						mesh->Indices.emplace_back(index.vertex_index);
						mesh->IndicesNormals.emplace_back(index.normal_index);
						mesh->IndicesUV.emplace_back(index.texcoord_index);

						//mesh->Vertices[index.vertex_index].UV = DirectX::XMFLOAT2(attrib.texcoords[2 * index.texcoord_index + 0],
						//	attrib.texcoords[2 * index.texcoord_index + 1]);
					}
					
					mesh->NumIndices = (u32)mesh->Indices.size();
					mesh->NumVertices = (u32)mesh->Vertices.size();
					mesh->Submeshes.emplace_back(submesh);
				}
			}

			if (!ResourceTable::TrackResource(ResourceType_StaticMesh, mesh))
			{
				delete mesh;
				return MeshHandle();
			}

			return Id;
		}

		DWN_CORE_WARN(warn);
		DWN_CORE_ERROR(err);

		return Id;
	}

	ShaderHandle RS_LoadShader(ResourceSystem* InFS, std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		ShaderHandle existingHandle = ResourceTable::LookUpResource(InMetaData->Type, InMetaData->Id);
		if (existingHandle.IsValid)
			return existingHandle;

		Shader* shader = new Shader();
		shader->FileId = InMetaData->Id;
		
		std::string combinedPath = (InWorkspacePath + InMetaData->Path + InMetaData->Name);
		HRESULT hr = GfxBackend::ReadShader(std::wstring(combinedPath.begin(), combinedPath.end()).c_str(), &shader->D3DData);

		if (FAILED(hr))
		{
			DWN_CORE_ERROR("Couldn't load compiled shader {0}", InMetaData->Name);
			delete shader;
			return ShaderHandle();
		}

		shader->Id = {};
		shader->Id.Index = (u32)ResourceTable::ResourceCount(ResourceType_Shader);
		shader->Id.IsValid = true;
		shader->Id.Generation = 0;

		if (!ResourceTable::TrackResource(ResourceType_Shader, shader))
		{
			delete shader;
			return ShaderHandle();
		}

		return shader->Id;
	}

	ImageHandle RS_LoadImage(ResourceSystem* InFS, std::string& InWorkspacePath, FileMetaData* InFile)
	{
		ImageHandle Id = ResourceTable::LookUpResource(InFile->Type, InFile->Id);
		if (Id.IsValid)
			return Id;

		std::string path = ToFullFilePath(InWorkspacePath, InFile);
		int x, y, n;
		unsigned char *data = stbi_load(path.c_str(), &x, &y, &n, 4);
		if (data != nullptr)
		{
			Image* image = new Image;
			image->FileId = InFile->Id;
			image->Width = x;
			image->Height = y;
			image->ChannelsPerPixel = n;
			image->Pixels = data;

			image->Id.Index = (u32)ResourceTable::ResourceCount(ResourceType_Image);
			image->Id.IsValid = true;

			if (!ResourceTable::TrackResource(ResourceType_Image, image))
			{
				stbi_image_free(data);
				delete image;
				return ImageHandle();
			}
			return image->Id;
		}

		return Id;
	}
}