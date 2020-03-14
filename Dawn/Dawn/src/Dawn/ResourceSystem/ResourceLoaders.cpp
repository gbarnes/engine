#include "stdafx.h"
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
#include <regex>

#include "Application.h"
#include "Core/GDI/inc_gfx.h"
#include "Core/GDI/Base/GfxShader.h"
#include "Core/GDI/Base/GfxBuffer.h"
#include "Core/GDI/Base/GfxVertexArrayObject.h"
#include "Core/Loading/File.h"
#include "Core/Logging/Log.h"
#include "Core/Paths.h"
#include "Rendering/RenderResourceHelper.h"
#include "Rendering/DeferredRenderer.h"

namespace Dawn
{

	void RS_ProcessMeshNode(ResourceSystem* InResourceSystem, Model* InModel, aiNode* InNode, const aiScene* InScene, vec3& currentMin, vec3& currentMax, HierarchyNode* parentNode = nullptr)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		const auto GDI = g_Application->GetGDI();
		
		if (InNode->mNumMeshes > 0)
		{
			ModelNodeData ModelNode;
			ModelNode.Name = InNode->mName.C_Str();
			if (InNode->mParent->mName == aiString(""))
			{
				aiVector3D pos, scale, rot;
				InNode->mParent->mTransformation.Decompose(scale, rot, pos);

				ModelNode.Position = vec3(pos.x, pos.y, pos.z);
				ModelNode.Scale = vec3(scale.x, scale.y, scale.z);
				ModelNode.Rotation = vec3(rot.x, rot.y, rot.z);
			}

			for (u32 i = 0; i < InNode->mNumMeshes; ++i)
			{
				const aiMesh* aiMesh = InScene->mMeshes[InNode->mMeshes[i]];

				Mesh* Mesh;
				InResourceSystem->CreateMesh(&Mesh);

				Mesh->Name = aiMesh->mName.C_Str();
				Mesh->NumIndices = aiMesh->mNumFaces * 3;
				Mesh->NumVertices = aiMesh->mNumVertices;

				vec3 min, max;
				min = max = vec3(aiMesh->mVertices[0].x, aiMesh->mVertices[0].y, aiMesh->mVertices[0].z);

				std::string Content;
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

					// x-axis
					if (pos->x < min.x)
						min.x = pos->x;
					if (pos->x > max.x)
						max.x = pos->x;

					// y-axis
					if (pos->y < min.y)
						min.y = pos->y;
					if (pos->y > max.y)
						max.y = pos->y;

					// z-axis
					if (pos->z < min.z)
						min.z = pos->z;
					if (pos->z > max.z)
						max.z = pos->z;
				}

				Mesh->Bounds.Max = max;
				Mesh->Bounds.Min = min;
				Mesh->Bounds.Extents = Mesh->Bounds.Max - Mesh->Bounds.Min;

				// check bounds of the entire model
				{
					// x-axis
					if (Mesh->Bounds.Min.x < currentMin.x)
						currentMin.x = Mesh->Bounds.Min.x;
					if (Mesh->Bounds.Max.x > currentMax.x)
						currentMax.x = Mesh->Bounds.Max.x;

					// y-axis
					if (Mesh->Bounds.Min.y < currentMin.y)
						currentMin.y = Mesh->Bounds.Min.y;
					if (Mesh->Bounds.Max.y > currentMax.y)
						currentMax.y = Mesh->Bounds.Max.y;

					// z-axis
					if (Mesh->Bounds.Min.z < currentMin.z)
						currentMin.z = Mesh->Bounds.Min.z;
					if (Mesh->Bounds.Max.z > currentMax.z)
						currentMax.z = Mesh->Bounds.Max.z;
				}

				Content = "";
				std::vector<u32> IndexData;
				for (u32 u = 0; u < aiMesh->mNumFaces; ++u)
				{
					const aiFace& Face = aiMesh->mFaces[u];
					assert(Face.mNumIndices == 3);
					IndexData.push_back(Face.mIndices[0]);
					IndexData.push_back(Face.mIndices[1]);
					IndexData.push_back(Face.mIndices[2]);

				}

				GfxVAOCreationData data = {};
				data.IndexBuffer.Data = &IndexData[0];
				data.IndexBuffer.Num = IndexData.size();
				data.IndexBuffer.Size = u32(IndexData.size() * sizeof(u32));
				data.IndexBuffer.StrideSize = sizeof(u32);

				data.VertexBuffer.Data = &VertexData[0];
				data.VertexBuffer.Num = VertexData.size();
				data.VertexBuffer.Size = u32(VertexData.size() * sizeof(float));
				data.VertexBuffer.StrideSize = u32(sizeof(float) * 10);

				auto* vao = CreateVAO(GDI, data);
				D_ASSERT(vao != nullptr, "Couldn't create vertex array object for loaded mesh!");
				Mesh->VertexArrayId = vao->GetId();

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
					//Material->Name

					aiString matname;
					aiGetMaterialString(aiMaterial, AI_MATKEY_NAME, &matname);
					Material->Name = matname.C_Str();
					//aiMaterial->

					aiColor4D albedo = aiColor4D(0, 0, 0, 0);
					aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &albedo);
					Material->Albedo = vec4(albedo.r, albedo.g, albedo.b, albedo.a);


					aiColor4D emissiveColor = aiColor4D(0, 0, 0, 0);
					aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_EMISSIVE, &emissiveColor);
					Material->Emissive = vec4(emissiveColor.r, emissiveColor.g, emissiveColor.b, emissiveColor.a);

					/*float reflection = 0;
					aiGetMaterialFloat(aiMaterial, AI_MATKEY_REFLECTIVITY, &reflection);
					Material->Metallic = reflection;

					float reflection = 0;
					aiGetMaterialFloat(aiMaterial, AI_MATKEY_REFLECTIVITY, &reflection);
					Material->Metallic = reflection;*/


					Material->ShaderId = CommonShaderHandles::Standard;
					Mesh->Materials.push_back(Material->Id);
				}

				InModel->Meshes.push_back(Mesh->Id);
				ModelNode.Meshes.push_back((i32)(InModel->Meshes.size() - 1));
			}

			parentNode = InModel->Hierarchy.AddNode(ModelNode, parentNode);
		}

		for (u32 i = 0; i < InNode->mNumChildren; ++i)
		{
			RS_ProcessMeshNode(InResourceSystem, InModel, InNode->mChildren[i], InScene, currentMin, currentMax, parentNode);
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
			aiProcess_Triangulate |  aiProcess_GenNormals | aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipWindingOrder | aiProcess_FixInfacingNormals
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			DWN_CORE_ERROR("Assimp Load error {0}", Importer.GetErrorString());
			return INVALID_HANDLE;
		}

		Model* model;
		Id = InResourceSystem->CreateModel(&model);
		model->FileId = InMetaData->Id;

		ModelNodeData ModelNode;
		ModelNode.Name = InMetaData->Name;

		aiVector3D pos, scale, rot;
		scene->mRootNode->mTransformation.Decompose(scale, rot, pos);

		ModelNode.Position = vec3(pos.x, pos.y, pos.z);
		ModelNode.Scale = vec3(scale.x, scale.y, scale.z);
		ModelNode.Rotation = vec3(rot.x, rot.y, rot.z);

		auto* RootNode = model->Hierarchy.AddNode(ModelNode);

		vec3 min, max;
		min = max = vec3(scene->mMeshes[0]->mVertices[0].x, scene->mMeshes[0]->mVertices[0].y, scene->mMeshes[0]->mVertices[0].z);
		
		ResourceId rid;
		rid.IsValid = false;

		RS_ProcessMeshNode(InResourceSystem, model, scene->mRootNode, scene, min, max, RootNode);

		model->Bounds.Max = max;
		model->Bounds.Min = min;
		model->Bounds.Extents = model->Bounds.Max - model->Bounds.Min;

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
		else
		{
			DWN_CORE_ERROR("Couldn't find file at path {0}", File.c_str());
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

	const std::set<std::string> PragmaKeywords = { "include"};
	const std::string PragmaDefine = "#pragma ";
	const std::string VersionDefine = "#version";
	bool ParseShaderFile(const std::string& InFilePath, const std::string& InRelPath, std::string* source)
	{
		bool bIsParsingVertexShader = false;
		bool bIsParsingFragShader = false;

		bool bIsShaderLoaded = ReadFileLineByLine(InFilePath, [&](const std::string& Line)
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
					if (PragmaKeyword == "include" && PragmaValue.size() > 0)
					{
						// include statments look like this #pragma include "filename" 
						// so we have to remove character from the front and back to get the path
						std::string IncludeName = PragmaValue.substr(2, PragmaValue.size() - 3);
						std::string SourceCode = LoadShaderInclude(Paths::ProjectContentDir().append(InRelPath).append(IncludeName).string());
						
						if (!SourceCode.empty())
						{
							source->append(SourceCode + "\n");
						}
					}
				}
			}
			else
			{
				source->append(Line + "\n");
			}
		});

		return bIsShaderLoaded;
	}

	GfxShaderType GetShaderTypeFromExt(const std::string& ext)
	{
		if (ext.find("ps") != std::string::npos)
		{
			return GfxShaderType::Pixel;
		}
		else if (ext.find("vs") != std::string::npos)
		{
			return GfxShaderType::Vertex;
		}
		else if (ext.find("hs") != std::string::npos)
		{
			return GfxShaderType::Hull;
		}
		else if (ext.find("gs") != std::string::npos)
		{
			return GfxShaderType::Geometry;
		}
		else if (ext.find("cs") != std::string::npos)
		{
			return GfxShaderType::Compute;
		}

		return GfxShaderType::Vertex;
	}

	const char* GetShaderProfileFromExt(const std::string& ext)
	{
		if (ext.find("ps") != std::string::npos)
		{
			return "ps_5_0";
		}
		else if (ext.find("vs") != std::string::npos)
		{
			return "vs_5_0";
		}
		else if (ext.find("hs") != std::string::npos)
		{
			return "hs_5_0";
		}
		else if (ext.find("gs") != std::string::npos)
		{
			return "gs_5_0";
		}
		else if (ext.find("cs") != std::string::npos)
		{
			return "cs_5_0";
		}

		return "vs_5_0";
	}
	
	ResourceId RS_LoadShader(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		const auto GDI = g_Application->GetGDI();
		ResourceId ShaderId = InResourceSystem->FindResourceIdFromFileId(InMetaData->Id);

		std::string FullPath = ToFullFilePath(InWorkspacePath, InMetaData);
	
		std::string source;
		bool isShaderLoaded = ParseShaderFile(FullPath, InMetaData->Path, &source);
		if (isShaderLoaded)
		{
			if (!ShaderId.IsValid) 
			{
				Shader* shader;
				InResourceSystem->CreateShader(&shader);
				D_ASSERT(shader != nullptr, "Couldn't create shader resource.");

				shader->FileId = InMetaData->Id;
				ShaderId = shader->Id;

				GfxShaderDesc desc;
				desc.FileName = InMetaData->Name.c_str();
				desc.Type = GetShaderTypeFromExt(InMetaData->Ext);
				desc.FunctionName = "Main";	 // note(gb): static for now!
				desc.ProfileName = GetShaderProfileFromExt(InMetaData->Ext); // note(gb): static for now!

				GfxShaderData data;
				data.Data = source.c_str();
				data.Size = source.size();

				GfxShader* gfxShader = nullptr;
				shader->GfxShaderId = GDI->CreateShader(desc, data, &gfxShader);
				D_ASSERT(gfxShader != nullptr, "Gfx shader null.");
			}
			else
			{
				auto shader = InResourceSystem->FindShader(ShaderId);
				D_ASSERT(shader != nullptr, "Shader resource not found by id.");

				GfxShader* gfxShader = GDI->GetShader(shader->GfxShaderId);
				D_ASSERT(gfxShader != nullptr, "No corresponding shader found.");

				GfxShaderDesc desc;
				desc.FileName = InMetaData->Name.c_str();
				desc.Type = GetShaderTypeFromExt(InMetaData->Ext);
				desc.FunctionName = "Main";	 // note(gb): static for now!
				desc.ProfileName = "vs_5_0"; // note(gb): static for now!

				GfxShaderData data;
				data.Data = source.c_str();
				data.Size = source.size();

				gfxShader->Create(desc, data);
			}
		}

		return ShaderId;
	}

	ResourceId RS_LoadImage(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InFile)
	{
		const auto Renderer = g_Application->GetRenderer();
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

			/*GfxTextureDesc Desc = 
			{
				GfxTextureFormat::RGBA32F,
				GfxMemoryType::UnsignedByte,
				GfxTextureFormat::RGBA32F,
				data, // raw pixel data
				(u32)x, // width of the image
				(u32)y, // height of the image
				(u16)n, // channels per pixel
				{ GL_REPEAT, GL_REPEAT }, // wrap settings 
				{ GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR }, // filter settings
				true
			};


			// these settings will later be filled by meta files
			// associated to the file
			image->TextureId = GDI->CreateTexture(Desc, nullptr);
			*/

			// Note(gb): remove this later since we don't want another dependency in the resource
			//			 system - maybe there shouldn't be a dependency to the gdi as well
			Renderer->Stats.TextureMemory += InFile->Size;

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
		const auto Renderer = g_Application->GetRenderer();
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
					
					/*GfxTextureDesc Desc =
					{
						GfxTextureFormat::RGBA32F,
						GfxMemoryType::UnsignedByte,
						GfxTextureFormat::RGBA32F,
						data, // raw pixel data
						static_cast<u32>(x), // width of the image
						static_cast<u32>(y), // height of the image
						static_cast<u16>(n), // channels per pixel
						{ GL_REPEAT, GL_REPEAT }, // wrap settings 
						{ GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR }, // filter settings
						true
					};

					// Note(gb): remove this later since we don't want another dependency in the resource
					//			 system - maybe there shouldn't be a dependency to the gdi as well
					Renderer->Stats.TextureMemory += InMetaData->Size;

					Texture->Reset(Desc);*/
					stbi_image_free(data);
				}
			}
		}
		

		return INVALID_HANDLE;
	}

	ResourceId RS_LoadLevel(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		static std::string EntityKeyword = "---Entity ";
		static std::string ComponentKeyword = "---Component ";

		ResourceId Id = InResourceSystem->FindResourceIdFromFileId(InMetaData->Id);
		if (Id.IsValid)
			return Id;

		bool bIsAtComponent = false;

		EntityData* Entity = nullptr;
		ComponentData* Component = nullptr;
		Level* Level;
		Id = InResourceSystem->CreateLevel(&Level);

		std::string FullPath = ToFullFilePath(InWorkspacePath, InMetaData);
		bool bIsLevelLoaded = ReadFileLineByLine(FullPath, [&](const std::string& Line)
		{
			std::string_view view(Line);

			// Begin Entity Parsing
			if (view.substr(0, EntityKeyword.size()) == EntityKeyword)
			{
				std::regex expression(".* \\{(.*)\\}\\{(.*)\\}\\{(.*)\\}");
				std::smatch match;

				bool bFound = std::regex_search(Line, match, expression);
				if (bFound)
				{
					std::string Guid = match.str(1);
					std::string Name = match.str(2);
					std::string ParentGuid = match.str(3);

					EntityData NewEntity;
					NewEntity.Guid = StringToUUID(Guid);
					NewEntity.Name = Name;
					NewEntity.ParentEntity = StringToUUID(ParentGuid);
					NewEntity.InFileId = InMetaData->Id;

					Level->Entities.push_back(NewEntity);
					Entity = &Level->Entities.at(Level->Entities.size() - 1);

					bIsAtComponent = false;
				}
			}

			// parse component variable
			if (bIsAtComponent)
			{
				std::regex expression("  ([a-zA-Z]*): (.*)");
				std::smatch match;

				bool bFound = std::regex_search(Line, match, expression);

				if (bFound)
				{
					std::string Name = match.str(1);
					std::string Data = match.str(2);


					ComponentData::ComponentValue ComponentVal;
					ComponentVal.VariableName = Name;
					ComponentVal.Data = Data;
					Component->ComponentValues.push_back(ComponentVal);
				}
			}

			// Begin Component Parsing!
			if (Entity != nullptr && view.substr(0, ComponentKeyword.size()) == ComponentKeyword)
			{
				std::regex expression(".* ([a-zA-Z]*)");
				std::smatch match;

				bool bFound = std::regex_search(Line, match, expression);
				if (bFound)
				{
					std::string type = match.str(1);

					ComponentData NewComponent;
					NewComponent.Type = type;

					Level->Components.push_back(NewComponent);
					u32 id = static_cast<u32>(Level->Components.size() - 1);

					Component = &Level->Components[id];
					Component->Id = id;

					Entity->IdToComponent.push_back(Component->Id);
					bIsAtComponent = true;
				}
			}
		});

		if (!bIsLevelLoaded)
			return INVALID_HANDLE;

		return Id;
	}

	ResourceId RS_ReloadLevel(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData)
	{
		DWN_CORE_WARN("No implementation to reload a level yet...");
		return INVALID_HANDLE;
	}
}