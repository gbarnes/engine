#include "ResourceLoaders.h"
#include "ResourceSystem.h"
#include "ResourceUtils.h"
#include <unordered_set>

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

	void RS_ProcessMeshNode(Model* InModel, aiNode* InNode, const aiScene* InScene)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		auto GDI = g_Application->GetGDI();

		for (u32 i = 0; i < InNode->mNumMeshes; ++i)
		{
			const aiMesh* aiMesh = InScene->mMeshes[InNode->mMeshes[i]];

			Mesh* mesh = new Mesh();
			mesh->Id.Index = (u32)ResourceTable::ResourceCount(ResourceType_StaticMesh);
			mesh->Id.Generation = 0;
			mesh->Id.IsValid = true;

			mesh->Name = aiMesh->mName.C_Str();
			mesh->NumIndices = aiMesh->mNumFaces * 3;
			mesh->NumVertices = aiMesh->mNumVertices;
			

			std::vector<float> VertexData;
			for (u32 y = 0; y < aiMesh->mNumVertices; ++y)
			{
				const aiVector3D* pos = &(aiMesh->mVertices[y]);
				const aiVector3D* normal = aiMesh->HasNormals() ? &(aiMesh->mNormals[y]) : &Zero3D;
				const aiVector3D* texCoord0 = aiMesh->HasTextureCoords(0) ? &(aiMesh->mTextureCoords[0][y]) : &Zero3D;
				const aiVector3D* texCoord1 = aiMesh->HasTextureCoords(1) ? &(aiMesh->mTextureCoords[1][y]) : &Zero3D;

				VertexData.push_back(pos->x);
				VertexData.push_back(pos->y);
				VertexData.push_back(pos->z);
				VertexData.push_back(normal->x);
				VertexData.push_back(normal->y);
				VertexData.push_back(normal->z);
				VertexData.push_back(texCoord0->x);
				VertexData.push_back(texCoord0->y);
				VertexData.push_back(texCoord1->x);
				VertexData.push_back(texCoord1->y);
			}

			std::vector<u32> IndexData;
			for (u32 u = 0; u < aiMesh->mNumFaces; ++u)
			{
				const aiFace& Face = aiMesh->mFaces[u];
				assert(Face.mNumIndices == 3);
				IndexData.push_back(Face.mIndices[0]);
				IndexData.push_back(Face.mIndices[1]);
				IndexData.push_back(Face.mIndices[2]);
			}

			GfxBufferLayout MeshLayout
			(
				{
					{ GfxShaderDataType::Float3, "position" },
					{ GfxShaderDataType::Float3, "normal" },
					{ GfxShaderDataType::Float2, "uv0" },
					{ GfxShaderDataType::Float2, "uv1" }
				}
			);

			// Buffer Setup
			GfxVertexArray* VertexArray = nullptr;
			auto VertexArrayId = GDI->CreateVertexArray(&VertexArray);

			GfxVertexBuffer* VertexBuffer;
			GDI->CreateVertexBuffer(&VertexData[0], VertexData.size() * sizeof(float), &VertexBuffer);
			VertexBuffer->SetLayout(MeshLayout);
			VertexArray->AttachVertexBuffer(VertexBuffer);

			GfxIndexBuffer* IndexBuffer;
			GDI->CreateIndexBuffer(&IndexData[0], IndexData.size(), &IndexBuffer);
			VertexArray->SetIndexBuffer(IndexBuffer);

			mesh->VertexArrayId = VertexArrayId;

			// Creating Material
			if (InScene->HasMaterials())
			{
				auto aiMaterial = InScene->mMaterials[aiMesh->mMaterialIndex];
				
				aiColor4D diffuseColor = aiColor4D(1, 1, 1, 1);
				aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);
				DWN_CORE_INFO("diffuse color: {0}, {1}, {2}, {3}", diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
			}

			if (!ResourceTable::TrackResource(ResourceType_StaticMesh, mesh))
			{
				GDI->ReturnIndexBuffer(IndexBuffer->GetId());
				GDI->ReturnVertexBuffer(VertexBuffer->GetId());
				GDI->ReturnVertexArray(VertexArray->GetId());
				delete mesh;
				continue;
			}

			InModel->Meshes.push_back(mesh->Id);
		}

		for (u32 i = 0; i < InNode->mNumChildren; ++i)
		{
			RS_ProcessMeshNode(InModel, InNode->mChildren[i], InScene);
		}
	}

	//
	// Loads a mesh with the assimp lib.
	//
	ModelHandle RS_LoadModel(ResourceSystem* InFS, std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		ModelHandle Id = ResourceTable::LookUpResource(InMetaData->Type, InMetaData->Id);
		if (Id.IsValid)
			return Id;

		std::string combinedPath = ToFullFilePath(InWorkspacePath, InMetaData);
		Assimp::Importer Importer;

		const aiScene* scene = Importer.ReadFile(combinedPath.c_str(),
			aiProcess_Triangulate | aiProcess_GenNormals  | aiProcessPreset_TargetRealtime_MaxQuality
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			DWN_CORE_ERROR("Assimp Load error {0}", Importer.GetErrorString());
			return INVALID_HANDLE;
		}

		Model* model = new Model();
		model->Id.Index = (u32)ResourceTable::ResourceCount(ResourceType_Model);
		model->Id.IsValid = true;
		model->Id.Generation = 0;
		model->FileId = InMetaData->Id;

		RS_ProcessMeshNode(model, scene->mRootNode, scene);
		
		if (!ResourceTable::TrackResource(ResourceType_Model, model))
		{
			delete model;
			return INVALID_HANDLE;
		}
		
		Id.IsValid = true;
		Id.Generation = 0;

		return Id;
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
		shader->ResourceId = g_Application->GetGDI()->CreateShader(&ResourceShader);

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