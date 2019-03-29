#include "ResourceLoaders.h"
#include "ResourceSystem.h"
#include "ResourceUtils.h"
#include <unordered_set>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "assimp/postprocess.h"
#include "assimp/types.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "glm.hpp"
#include "pugixml.hpp"

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

			for (u32 i = 0; i < scene->mNumMeshes; i++) 
			{
				const aiMesh* aiMesh = scene->mMeshes[i];
				Submesh submesh = {};
				submesh.Name = aiMesh->mName.C_Str();
				submesh.NumOfIndices = aiMesh->mNumFaces * 3;

				for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) 
				{
					const aiVector3D* pos = &(aiMesh->mVertices[i]);
					const aiVector3D* normal = aiMesh->HasNormals() ? &(aiMesh->mNormals[i]) : &Zero3D;
					const aiVector3D* texCoord0 = aiMesh->HasTextureCoords(0) ? &(aiMesh->mTextureCoords[0][i]) : &Zero3D;
					const aiVector3D* texCoord1 = aiMesh->HasTextureCoords(1) ? &(aiMesh->mTextureCoords[0][i]) : &Zero3D;

					VertexPosUV vertex = {};
					vertex.Position = vec3(pos->x, pos->y, pos->z);
					vertex.UV0 = vec2(texCoord0->x, texCoord0->y);
					vertex.UV1 = vec2(texCoord1->x, texCoord1->y);
					mesh->Vertices.push_back(vertex);
				}

				for (unsigned int i = 0; i < aiMesh->mNumFaces; i++) 
				{
					const aiFace& Face = aiMesh->mFaces[i];
					assert(Face.mNumIndices == 3);
					mesh->Indices.push_back(Face.mIndices[0]);
					mesh->Indices.push_back(Face.mIndices[1]);
					mesh->Indices.push_back(Face.mIndices[2]);
				}

				submesh.StartIndex = (u32)std::max((i32)mesh->Indices.size() - 1, 0);
				mesh->Submeshes.push_back(submesh);
				mesh->NumVertices += aiMesh->mNumVertices;
				mesh->NumIndices += aiMesh->mNumFaces * 3;
			}

			if (!ResourceTable::TrackResource(ResourceType_StaticMesh, mesh))
			{
				delete mesh;
				return MeshHandle();
			}

			return Id;
		}
		else 
		{
			DWN_CORE_WARN("Error parsing '{0}': '{0}'", InMetaData->Name, Importer.GetErrorString());
		}


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
			return existingHandle;
		}


		Shader* shader = new Shader();
		shader->FileId = InMetaData->Id;
		shader->Id = {};
		shader->Id.Index = (u32)ResourceTable::ResourceCount(ResourceType_Shader);
		shader->Id.IsValid = true;
		shader->Id.Generation = 0;
		shader->GDI_ShaderId = glCreateProgram();
		
		std::vector<u32> shadersToDelete;
		for (auto step : doc.child("shader").children())
		{
			auto typeAttr = std::string(step.attribute("type").as_string());
			bool isPixelShader = (typeAttr == "ps");
			
			auto shaderBuffer = step.text().as_string();
			auto shaderId = glCreateShader(isPixelShader ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER); // refactor this into GDI layer!
			shadersToDelete.emplace_back(shaderId);

			glShaderSource(shaderId, 1, &shaderBuffer, NULL);
			glCompileShader(shaderId);

			int  success;
			char infoLog[512];
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
				DWN_CORE_ERROR("ERROR::SHADER::COMPILATION_FAILED {0} \n", std::string(infoLog));

				for (auto shaderToDelete : shadersToDelete)
					glDeleteShader(shaderToDelete);

				return ShaderHandle();
			}
			else
			{
				glAttachShader(shader->GDI_ShaderId, shaderId);
			}
		}

		glLinkProgram(shader->GDI_ShaderId);

		for (auto shaderToDelete : shadersToDelete)
			glDeleteShader(shaderToDelete);

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
		unsigned char *data = stbi_load(path.c_str(), &x, &y, &n, 0);
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