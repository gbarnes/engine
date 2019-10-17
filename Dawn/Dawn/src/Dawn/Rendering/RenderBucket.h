#pragma once
#include "inc_common.h"
#include "Core/GDI/inc_gfx.h"
#include "inc_renderer_common.h"
#include "CommandPacket.h"
#include "Application.h"
#include "EntitySystem/Lights/LightComponents.h"
#include "EntitySystem/Entity.h"


namespace Dawn
{
	class Application;

	template<class T>
	class DAWN_API RenderBucket
	{
		typedef T Key;
	public:
		RenderBucket()
		{}

		void Free()
		{
			// super duper stupid - this should be better with a linear allocator for the commands.
			std::vector<CommandPacket> Packets;
			for (u32 i = 0; i < CommandCount; ++i)
			{
				CommandPacket packet = Data[i];

				do
				{
					Packets.push_back(packet);
					packet = commandPacket::LoadNextCommandPacket(packet);
					
				} while (packet != nullptr);
			}

			for (auto Packet : Packets)
				delete Packet;

			CommandCount = 0;

			delete[] Keys;
			delete[] Data;
		}

		void Reset(u32 InMaxSize, GfxResId InFrameBuffer, const mat4& InViewMatrix, const mat4& InProjMatrix)
		{
			RenderBufferId = InFrameBuffer;
			Keys = new Key[InMaxSize];
			Data = new CommandPacket[InMaxSize];
			Projection = InProjMatrix;
			View = InViewMatrix;
		}

		template<typename U>
		U* AddCommand(Key InKey, size_t auxMemorySize = 0u)
		{
			CommandPacket packet = commandPacket::Create<U>(auxMemorySize);
			
			{
				const u32 Current = CommandCount++;
				Keys[Current] = InKey;
				Data[Current] = packet;
			}

			commandPacket::StoreNextCommandPacket(packet, nullptr);
			commandPacket::StoreBackendDispatchFunction(packet, U::DRAW_FUNCTION);

			return commandPacket::GetCommand<U>(packet);
		}

		template <typename U, typename V>
		U* AppendCommand(V* command, size_t auxMemorySize = 0u)
		{
			CommandPacket packet = commandPacket::Create<U>(auxMemorySize);

			// append this command to the given one
			commandPacket::StoreNextCommandPacket<V>(command, packet);

			commandPacket::StoreNextCommandPacket(packet, nullptr);
			commandPacket::StoreBackendDispatchFunction(packet, U::DRAW_FUNCTION);

			return commandPacket::GetCommand<U>(packet);
		}

		template<typename U>
		U* AllocateCommand()
		{
			return new U();
		}

		void Sort()
		{
		}

		void Submit(Application* InApp)
		{
			const Shared<GfxGDI> GDI = InApp->GetGDI();
			const Shared<ResourceSystem> Resources = InApp->GetResourceSystem();
			const Shared<World> World = InApp->GetWorld();

			ScopedGfxBind<GfxRenderBuffer> Binder;
			if (RenderBufferId.IsValid)
			{
				auto Buffer = GDI->GetRenderBuffer(RenderBufferId);
				Binder.Bind(Buffer);
			}

			u32 LastMatId = 1232145124;
			GfxShader* GfxShader = nullptr;

			for (u32 i = 0; i < CommandCount; ++i)
			{
				Key key = Keys[i];
				CommandPacket packet = Data[i];

				DrawKey64 DecodedKey = DecodeDrawKey64(key);

				if (DecodedKey.bIsValid && DecodedKey.MaterialId != LastMatId)
				{
					if (GfxShader != nullptr)
						GfxShader->Unbind();

					// note --- uhm we have no way to extract the generation for the material
					//			think about this - do we need the generation? is this ever going
					//			to change?! (gb, 10/17/19)
					ResourceId ResId;
					ResId.Generation = 1;
					ResId.Index = DecodedKey.MaterialId;
					ResId.IsValid = true;

					auto Material = Resources->FindMaterial(ResId);
					auto Shader = Resources->FindShader(Material->ShaderId);
					GfxShader = GDI->GetShader(Shader->ResourceId);
					GfxShader->Bind();

					GfxShader->SetMat4("proj", Projection);
					GfxShader->SetMat4("view", View);

					GfxShader->SetVec4("material.diffuse", Material->DiffuseColor);
					GfxShader->SetVec4("material.ambient", Material->AmbientColor);
					GfxShader->SetVec4("material.specular", Material->SpecularColor);
					GfxShader->SetFloat("material.shininess", 1.0f);

					// remove this again! this is only here as a test
					// but since we use a deferred renderer later this should really
					// be set by the lighting pass!
					
					ComponentId Id;
					Id.Index = 0;
					Id.Generation = 1;
					Id.IsValid = true;
					auto Light = World->GetComponentById<DirectionalLight>(Id);
					auto LightTransform = World->GetComponentByEntity<Transform>(Light->GetEntity()->Id);

					GfxShader->SetVec4("light.diffuse", Light->Color);
					GfxShader->SetVec3("light.position", LightTransform->Position);

					LastMatId = DecodedKey.MaterialId;
				}

				do
				{
					const DrawDispatchFunction function = commandPacket::LoadBackendDispatchFunction(packet);
					const void* command = commandPacket::LoadCommand(packet);
					function(GDI.get(), GfxShader, command);

					packet = commandPacket::LoadNextCommandPacket(packet);
				} while (packet != nullptr);
			}

			if (GfxShader != nullptr)
				GfxShader->Unbind();
		}

	private:
		GfxResId RenderBufferId;
		Key* Keys;
		CommandPacket* Data;
		std::atomic<u32> CommandCount;

		mat4 Projection;
		mat4 View;
	};

}