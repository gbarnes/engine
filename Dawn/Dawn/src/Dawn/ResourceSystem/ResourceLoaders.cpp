#include "ResourceLoaders.h"
#include "ResourceSystem.h"
#include "ResourceUtils.h"
#include <unordered_set>
#include <fstream>

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
#include "Rendering/RenderResourceHelper.h"


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
				Material->ShaderId = CommonShaderHandles::Standard;
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
			aiProcess_Triangulate | aiProcess_GenNormals | aiProcessPreset_TargetRealtime_MaxQuality
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

	bool ReadFileLineByLine(const std::string& File, std::function<void(std::string&)> InProcessingFunc)
	{
		std::ifstream file(File);
		if (file.is_open()) {
			std::string line;
			while (getline(file, line))
			{
				if (InProcessingFunc != nullptr)
					InProcessingFunc(std::string(line));
			}
			file.close();
			return true;
		}
	

		return false;
	}

	std::string LoadShaderInclude(const std::string& InFile)
	{
		std::string IncludedFile;

		ReadFileLineByLine(InFile, [&](const std::string& Line) 
		{
			IncludedFile.append(Line + "\n");
		});

		return IncludedFile;
	}

	const std::set<std::string> PragmaKeywords = { "vert_begin", "include", "frag_begin" };
	const std::string PragmaDefine = "#pragma ";
	const std::string VersionDefine = "#version";
	bool ParseShaderFile(const std::string& InPath, std::string* OutVersionString, std::string* OutVertexSource, std::string* OutFragSource)
	{
		bool bIsParsingVertexShader = false;
		bool bIsParsingFragShader = false;

		bool bIsShaderLoaded = ReadFileLineByLine(InPath, [&](const std::string& Line)
		{
			if (Line.substr(0, PragmaDefine.size()) == PragmaDefine)
			{
				std::string PragmaKeyword = Line.substr(PragmaDefine.size());
				std::string PragmaValue;
				std::string::size_type PosSpace = PragmaKeyword.find_first_of(' ');

				if (PosSpace != std::string::npos)
				{
					PragmaValue = PragmaKeyword.substr(PosSpace, PragmaKeyword.size());
					PragmaKeyword = PragmaKeyword.substr(0, PosSpace);
				}

				if (PragmaKeywords.find(PragmaKeyword) != PragmaKeywords.end())
				{
					if (PragmaKeyword == "vert_begin")
					{
						bIsParsingFragShader = false;
						bIsParsingVertexShader = true;
					}
					else if (PragmaKeyword == "frag_begin")
					{
						bIsParsingVertexShader = false;
						bIsParsingFragShader = true;
					}
					else if (PragmaKeyword == "include" && PragmaValue.size() > 2 && (bIsParsingVertexShader || bIsParsingFragShader))
					{
						// include statments look like this #pragma include "filename" 
						// so we have to remove character from the front and back to get the path
						std::string IncludeName = PragmaValue.substr(2, PragmaValue.size() - 3);
						std::string SourceCode = LoadShaderInclude(Paths::ProjectShaderDir().append(IncludeName).string());
						
						if (!SourceCode.empty())
						{
							if (bIsParsingVertexShader)
								OutVertexSource->append(SourceCode + "\n");
							else if (bIsParsingFragShader)
								OutFragSource->append(SourceCode + "\n");
						}
					}
				}
			}
			else if (Line.substr(0, VersionDefine.size()) == VersionDefine)
			{
				*OutVersionString = Line + "\n";
			}
			else
			{
				if (bIsParsingVertexShader)
					OutVertexSource->append(Line + "\n");
				else if (bIsParsingFragShader)
					OutFragSource->append(Line + "\n");
			}
		});

		return bIsShaderLoaded;
	}
	


	ResourceId RS_LoadShader(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		const auto GDI = g_Application->GetGDI();
		ResourceId ShaderId = InResourceSystem->FindResourceIdFromFileId(InMetaData->Id);

		std::string FullPath = ToFullFilePath(InWorkspacePath, InMetaData);

		// Todo -- add other shader types later on ... and we of course should be able to somehow use 
		//		   this with directx (hlsl) too once we get that back in (gb, 10/12/19).
		
		std::string VertexSource = "";
		std::string FragSource = "";
		std::string VersionString = "";

		bool bIsShaderLoaded = ParseShaderFile(FullPath, &VersionString, &VertexSource, &FragSource);

		if (bIsShaderLoaded)
		{
			std::string FinalVertexShader = VersionString + VertexSource;
			std::string FinalPixelShader = VersionString + FragSource;

			GfxShader* InternalShader = nullptr;
			if (!ShaderId.IsValid)
			{
				Shader* shader;
				InResourceSystem->CreateShader(&shader);

				shader->FileId = InMetaData->Id;
				ShaderId = shader->Id;
				shader->ResourceId = GDI->CreateShader(&InternalShader);
			}
			else
			{
				auto Shader = InResourceSystem->FindShader(ShaderId);
				InternalShader = GDI->GetShader(Shader->ResourceId);
			}

			assert(InternalShader != nullptr && "Gfx shader null.");
			InternalShader->AttachSource(GfxShaderType::ST_Vertex, FinalVertexShader.c_str());
			InternalShader->AttachSource(GfxShaderType::ST_Pixel, FinalPixelShader.c_str());
		}

		return ShaderId;
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