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

	void RS_ProcessMeshNode(ResourceSystem* InResourceSystem, Model* InModel, aiNode* InNode, const aiScene* InScene)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		const auto GDI = g_Application->GetGDI();
		
		for (u32 i = 0; i < InNode->mNumMeshes; ++i)
		{
			const aiMesh* aiMesh = InScene->mMeshes[InNode->mMeshes[i]];

			Mesh* Mesh;
			InResourceSystem->CreateMesh(&Mesh);

			Mesh->Name = aiMesh->mName.C_Str();
			Mesh->NumIndices = aiMesh->mNumFaces * 3;
			Mesh->NumVertices = aiMesh->mNumVertices;
			

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

			Mesh->VertexArrayId = VertexArrayId;

			// Creating Material
			if (InScene->HasMaterials())
			{
				Material* Material;
				InResourceSystem->CreateMaterial(&Material);

				const auto aiMaterial = InScene->mMaterials[aiMesh->mMaterialIndex];
				
				// TODO (gb): add texture loading and path handling, asset conversion to a custom binary format etc.
				/*u32 diffuseTextureCount = aiGetMaterialTextureCount(aiMaterial, aiTextureType_DIFFUSE);
				DWN_CORE_INFO("Diffuse Texture count: {0}", diffuseTextureCount);

				aiString path;
				aiGetMaterialTexture(aiMaterial, aiTextureType_DIFFUSE, 0, &path);
				DWN_CORE_INFO("Texture Path: {0}", path.C_Str());*/


				aiColor4D diffuseColor = aiColor4D(1, 1, 1, 1);
				aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);
				Material->DiffuseColor = vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);

				aiColor4D specularColor = aiColor4D(1, 1, 1, 1);
				aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &specularColor);
				Material->SpecularColor = vec4(specularColor.r, specularColor.g, specularColor.b, specularColor.a);

				aiColor4D ambientColor = aiColor4D(1, 1, 1, 1);
				aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &specularColor);
				Material->AmbientColor = vec4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a);

				float shinieness = 0;
				aiGetMaterialFloat(aiMaterial, AI_MATKEY_SHININESS, &shinieness);
				Material->Shinieness = shinieness;

				Mesh->Materials.push_back(Material->Id);
			}

			InModel->Meshes.push_back(Mesh->Id);
		}

		for (u32 i = 0; i < InNode->mNumChildren; ++i)
		{
			RS_ProcessMeshNode(InResourceSystem, InModel, InNode->mChildren[i], InScene);
		}
	}

	//
	// Loads a mesh with the assimp lib.
	//
	ResourceId RS_LoadModel(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		ResourceId Id = InResourceSystem->FindResourceIdFromFileId(InMetaData->Id);
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

		Model* model;
		Id = InResourceSystem->CreateModel(&model);
		model->FileId = InMetaData->Id;

		RS_ProcessMeshNode(InResourceSystem, model, scene->mRootNode, scene);

		return Id;
	}

	ResourceId RS_LoadShader(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		ResourceId Id = InResourceSystem->FindResourceIdFromFileId(InMetaData->Id);
		if (Id.IsValid)
			return Id;

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

		Shader* shader;
		InResourceSystem->CreateShader(&shader);

		shader->FileId = InMetaData->Id;

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

		return shader->Id;
	}

	ResourceId RS_LoadImage(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InFile)
	{
		const auto GDI = g_Application->GetGDI();

		ResourceId Id = InResourceSystem->FindResourceIdFromFileId(InFile->Id);
		if (Id.IsValid)
			return Id;

		std::string path = ToFullFilePath(InWorkspacePath, InFile);
		int x, y, n;
		unsigned char *data = stbi_load(path.c_str(), &x, &y, &n, 4);
		
		
		if (data != nullptr)
		{
			//stbi__vertical_flip(data, x, y, n);
			Image* image;
			Id = InResourceSystem->CreateImage(&image);

			image->FileId = InFile->Id;
			image->Width = x;
			image->Height = y;
			image->ChannelsPerPixel = n;

			// these settings will later be filled by meta files
			// associated to the file
			image->TextureId = GDI->CreateTexture
			(
				data, // raw pixel data
				x, // width of the image
				y, // height of the image
				n, // channels per pixel
				{ GL_REPEAT, GL_REPEAT }, // wrap settings 
				{ GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR }, // filter settings
				true, // generate mip maps?
				nullptr // the output pointer to the created texture!
			);
			
			// hm is it good to release the data...
			stbi_image_free(data);

			return Id;
		}
		else
		{
			DWN_CORE_ERROR("{0} ({1}) ", stbi_failure_reason(), InFile->Name);
		}

		return INVALID_HANDLE;
	}


	ResourceId RS_ReloadModel(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		DWN_CORE_WARN("No implementation to reload a model yet...");
		return INVALID_HANDLE;
	}

	ResourceId RS_ReloadShader(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		const auto GDI = g_Application->GetGDI();
		auto ResourceId = InResourceSystem->FindResourceIdFromFileId(InMetaData->Id);
		auto CastedResource = InResourceSystem->FindShader(ResourceId);

		if (CastedResource->ResourceId.IsValid)
		{
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

			GfxShader* shader = GDI->GetShader(CastedResource->ResourceId);
			if (shader)
			{
				std::vector<u32> shadersToDelete;
				for (auto step : doc.child("shader").children())
				{
					auto typeAttr = std::string(step.attribute("type").as_string());
					bool isPixelShader = (typeAttr == "ps");

					auto shaderBuffer = step.text().as_string();
					shader->AttachSource(isPixelShader ? GfxShaderType::ST_Pixel : GfxShaderType::ST_Vertex, shaderBuffer);
				}
			}
		}

		return INVALID_HANDLE;
	}

	ResourceId RS_ReloadImage(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		const auto GDI = g_Application->GetGDI();
		auto ResourceId = InResourceSystem->FindResourceIdFromFileId(InMetaData->Id);
		auto CastedResource = InResourceSystem->FindImage(ResourceId);

		if (CastedResource->TextureId.IsValid)
		{
			auto Texture = GDI->GetTexture(CastedResource->TextureId);
			if (Texture)
			{
				int x, y, n;

				std::string path = ToFullFilePath(InWorkspacePath, InMetaData);
				unsigned char *data = stbi_load(path.c_str(), &x, &y, &n, 4);
				if (data)
				{
					Texture->Reset(data, x, y, n, { GL_REPEAT, GL_REPEAT }, { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR }, true);
					stbi_image_free(data);
				}
			}
		}
		

		return INVALID_HANDLE;
	}

}