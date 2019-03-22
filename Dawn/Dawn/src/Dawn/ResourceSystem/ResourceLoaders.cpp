#include "ResourceLoaders.h"
#include "ResourceSystem.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader.h>

namespace Dawn
{
	MeshHandle LoadStaticMesh(ResourceSystem* InFS, std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		GenericHandle invalidHandle;
		invalidHandle.IsValid = false;

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
			u32 meshIndex = ResourceSystem::FreeResourceIndex(ResourceType_StaticMesh);
		
			MeshHandle Id = {};
			Id.Index = meshIndex;
			
			// The generation can always be 0 in this case since we don't allow 
			// to remove meshes while runtime at this time! - Gavin Barnes 03/22/19
			Id.Generation = 0;

			Id.IsValid = true;

			Mesh mesh = {};
			mesh.Id = Id;
			mesh.FileId = InMetaData->Id;
			
			// load vertices and save to the mesh!
			{			
				// divide by three because we have three vertices per triangle!
				// we assume that the mesh is being triangulated which we told tinyobj when loading!
				u128 numOfVertices = attrib.vertices.size() / 3;

				for (u32 i = 0; i < numOfVertices; ++i)
				{
					//VertexPosNormalUV vertex = {};
					
					VertexPosColor vertex = {};
					vertex.Position = DirectX::XMFLOAT3(attrib.vertices[3 * i], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]);

					//if(attrib.normals.size() > 0)
					//	vertex.Normal = DirectX::XMFLOAT3(attrib.normals[3 * i], attrib.normals[3 * i + 1], attrib.normals[3 * i + 2]);

					float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					vertex.Color = DirectX::XMFLOAT3(r, r, r);

					mesh.Vertices.emplace_back(vertex);
				}
			}

			// load indices and submesh groups!
			{
				for (auto shape : shapes)
				{
					Submesh submesh = {};
					submesh.Name = shape.name;
					submesh.NumOfIndices = shape.mesh.indices.size();
					submesh.StartIndex = (u32)std::max((i32)mesh.Indices.size() - 1, 0);

					for (auto index : shape.mesh.indices)
					{
						mesh.Indices.emplace_back(index.vertex_index);
						mesh.IndicesNormals.emplace_back(index.normal_index);
						mesh.IndicesUV.emplace_back(index.texcoord_index);

						//mesh.Vertices[index.vertex_index].UV = DirectX::XMFLOAT2(attrib.texcoords[2 * index.texcoord_index + 0],
						//	attrib.texcoords[2 * index.texcoord_index + 1]);
					}

					mesh.NumIndices = mesh.Indices.size();
					mesh.NumVertices = mesh.Vertices.size();
					mesh.Submeshes.emplace_back(submesh);
				}
			}

			ResourceSystem::RegisterResource(ResourceType_StaticMesh, &mesh);

			return Id;
		}

		DWN_CORE_WARN(warn);
		DWN_CORE_ERROR(err);

		return invalidHandle;
	}
}