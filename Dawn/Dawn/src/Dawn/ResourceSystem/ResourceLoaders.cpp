#include "ResourceLoaders.h"
#include "ResourceSystem.h"
#include "ResourceUtils.h"
#include <unordered_set>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG 
#include <stb_image.h>

#include "assimp/postprocess.h"
#include "assimp/types.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "glm.hpp"
#include "pugixml.hpp"

#include "Application.h"
#include "Core/GDI/inc_gfx.h"
#include "Core/Loading/File.h"


namespace Dawn
{
	//
	// Loads a mesh with the assimp lib.
	//
	MeshHandle RS_LoadStaticMeshWithAssimp(ResourceSystem* InFS, std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		MeshHandle Id = ResourceTable::LookUpResource(InMetaData->Type, InMetaData->Id);
		if (Id.IsValid)
			return Id;

		std::string combinedPath = ToFullFilePath(InWorkspacePath, InMetaData);
		Assimp::Importer Importer;

		const aiScene* scene = Importer.ReadFile(combinedPath.c_str(),
			aiProcess_Triangulate | aiProcess_GenSmoothNormals | 
			 aiProcess_JoinIdenticalVertices
		);

		if (scene)
		{
			// setup mesh handle
			{
				u32 meshIndex = (u32)ResourceTable::ResourceCount(ResourceType_StaticMesh);
				Id.Index = meshIndex;
				Id.IsValid = true;
				Id.Generation = 0;
			}

			// Implement more error checks - gbarnes, 03/26/19

			Mesh* mesh = new Mesh();
			mesh->Id = Id;
			mesh->FileId = InMetaData->Id;

			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

			GfxBufferLayout MeshLayout
			(
				{ 
					{ GfxShaderDataType::Float3, "aPos" },
					{ GfxShaderDataType::Float2, "aUV0" },
					{ GfxShaderDataType::Float2, "aUV1" }
				}
			);

			GfxVertexArray* VertexArray = nullptr;
			auto VertexArrayId = GfxGDI::Get()->CreateVertexArray(&VertexArray);
			
			std::vector<u16> IndexData;

			for (u32 i = 0; i < scene->mNumMeshes; i++) 
			{
				const aiMesh* aiMesh = scene->mMeshes[i];
				Submesh submesh = {};
				submesh.Name = aiMesh->mName.C_Str();
				submesh.NumOfIndices = aiMesh->mNumFaces * 3;

				u32 componentCount = 7;
				float* verts = new float[aiMesh->mNumVertices * componentCount]; 
				int x = -1;
				for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
				{
					const aiVector3D* pos = &(aiMesh->mVertices[i]);
					const aiVector3D* normal = aiMesh->HasNormals() ? &(aiMesh->mNormals[i]) : &Zero3D;
					const aiVector3D* texCoord0 = aiMesh->HasTextureCoords(0) ? &(aiMesh->mTextureCoords[0][i]) : &Zero3D;
					const aiVector3D* texCoord1 = aiMesh->HasTextureCoords(1) ? &(aiMesh->mTextureCoords[1][i]) : &Zero3D;

					verts[++x] = pos->x;
					verts[++x] = pos->y;
					verts[++x] = pos->z;
					verts[++x] = texCoord0->x;
					verts[++x] = texCoord0->y;
					verts[++x] = texCoord1->x;
					verts[++x] = texCoord1->y;
				}
				
				 
				GfxVertexBuffer* Buffer;
				GfxGDI::Get()->CreateVertexBuffer(verts, aiMesh->mNumVertices * componentCount * sizeof(float), &Buffer);
				Buffer->SetLayout(MeshLayout);
				VertexArray->AttachVertexBuffer(Buffer);

				delete[] verts;

				for (unsigned int i = 0; i < aiMesh->mNumFaces; i++) 
				{
					const aiFace& Face = aiMesh->mFaces[i];
					assert(Face.mNumIndices == 3);
					IndexData.push_back(Face.mIndices[0]);
					IndexData.push_back(Face.mIndices[1]);
					IndexData.push_back(Face.mIndices[2]);
				}

				submesh.StartIndex = (u32)std::max((i32)IndexData.size() - 1, 0);
				mesh->Submeshes.push_back(submesh);
				mesh->NumVertices += (u32)aiMesh->mNumVertices;
				mesh->NumIndices += submesh.NumOfIndices;
			}

			GfxIndexBuffer* IndexBuffer;
			GfxGDI::Get()->CreateIndexBuffer(&IndexData[0], mesh->NumIndices, &IndexBuffer);
			VertexArray->SetIndexBuffer(IndexBuffer);

			mesh->VertexArray = VertexArrayId;

			if (!ResourceTable::TrackResource(ResourceType_StaticMesh, mesh))
			{
				delete mesh;
				return INVALID_HANDLE;
			}

			return Id;
		}
		else 
		{
			DWN_CORE_WARN("Error parsing '{0}': '{0}'", InMetaData->Name, Importer.GetErrorString());
		}


		return INVALID_HANDLE;
	}

	ShaderHandle RS_LoadShader(ResourceSystem* InFS, std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		ShaderHandle existingHandle = ResourceTable::LookUpResource(InMetaData->Type, InMetaData->Id);
		if (existingHandle.IsValid)
			return existingHandle;

		std::string combinedPath = ToFullFilePath(InWorkspacePath, InMetaData);

		// This is a temporary solution to be able to load ps and vs shader at 
		// the same time. I guess once I switch back to DX12 or DX11 it should be
		// different. - gbarnes 03/29/19

		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(combinedPath.c_str());
		if (!result)
		{
			DWN_CORE_ERROR("Couldn't parse shader xml!");
			return INVALID_HANDLE;
		}

		Shader* shader = new Shader();
		shader->FileId = InMetaData->Id;
		shader->Id = {};
		shader->Id.Index = (u32)ResourceTable::ResourceCount(ResourceType_Shader);
		shader->Id.IsValid = true;
		shader->Id.Generation = 0;

		GfxShader* ResourceShader;
		shader->ResourceId = GfxGDI::Get()->CreateShader(&ResourceShader);

		std::vector<u32> shadersToDelete;
		for (auto step : doc.child("shader").children())
		{
			auto typeAttr = std::string(step.attribute("type").as_string());
			bool isPixelShader = (typeAttr == "ps");
			
			auto shaderBuffer = step.text().as_string();
			ResourceShader->AttachSource(isPixelShader ? GfxShaderType::ST_Pixel : GfxShaderType::ST_Vertex, shaderBuffer);
		}

		if (!ResourceTable::TrackResource(ResourceType_Shader, shader))
		{
			delete shader;
			return INVALID_HANDLE;
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
			stbi__vertical_flip(data, x, y, n);
			Image* image = new Image;
			image->FileId = InFile->Id;
			image->Width = x;
			image->Height = y;
			image->ChannelsPerPixel = n;
			image->Pixels = data;

			image->Id.Index = (u32)ResourceTable::ResourceCount(ResourceType_Image);
			image->Id.IsValid = true;
			image->Id.Generation = 0;

			
			if (!ResourceTable::TrackResource(ResourceType_Image, image))
			{
				stbi_image_free(data);
				delete image;
				return ImageHandle();
			}

			//stbi_image_free(data);
			return image->Id;
		}
		else
		{
			DWN_CORE_ERROR("{0} ({1}) ", stbi_failure_reason(), InFile->Name);
		}

		return INVALID_HANDLE;
	}
}