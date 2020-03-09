#pragma once
#include "Core/GDI/inc_gfx.h"
#include "inc_renderer_common.h"
#include "CommandPacket.h"
#include "Core/Memory/MemoryAllocators.h"
#include "Renderer.h"
#include "Application.h"
#include "ResourceSystem/ResourceSystem.h"
#include "EntitySystem/Lights/LightComponents.h"
#include "EntitySystem/Entity.h"
#include "RenderResourceHelper.h"

namespace Dawn
{
	class Application;

	template<class T>
	class DAWN_API RenderBucket
	{
		typedef T Key;
	public:
		RenderBucket()
			: Keys(nullptr), Data(nullptr)
		{
			HeapMemory = Memory::HeapArea(_2MB);
			Allocator = Allocators::LinearAllocator(HeapMemory.GetStart(), HeapMemory.GetEnd());
		}

		~RenderBucket()
		{
			HeapMemory.Free();
			delete[] Keys;
			delete[] Data;
		}

		void Free()
		{
			Allocator.Reset();
			CommandCount = 0;
		}

		void Reset(u32 InMaxSize, GfxRTBundle* InBundle, const mat4& InViewMatrix, const mat4& InProjMatrix)
		{
			RenderBundle = InBundle;

			if (Keys == nullptr && Data == nullptr)
			{
				Keys = new Key[InMaxSize];
				Data = new CommandPacket[InMaxSize];
			}

			CommandCount = 0;
			Projection = InProjMatrix;
			View = InViewMatrix;
		}

		template<typename U>
		U* AddCommand(Key InKey, size_t auxMemorySize = 0u)
		{
			CommandPacket packet = commandPacket::Create<U>(Allocator, auxMemorySize);
			
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
			CommandPacket packet = commandPacket::Create<U>(Allocator, auxMemorySize);

			// append this command to the given one
			commandPacket::StoreNextCommandPacket<V>(command, packet);

			commandPacket::StoreNextCommandPacket(packet, nullptr);
			commandPacket::StoreBackendDispatchFunction(packet, U::DRAW_FUNCTION);

			return commandPacket::GetCommand<U>(packet);
		}

		template<typename U>
		U* AllocateCommand()
		{
			return D_NEW(U, AllocationArena);
		}

		void Sort()
		{
		}

		void Submit(Application* InApp)
		{
			const Shared<GfxGDI> GDI = InApp->GetGDI();
			const Shared<ResourceSystem> Resources = InApp->GetResourceSystem();
			const Shared<World> World = InApp->GetWorld();
			const Shared<DeferredRenderer> Renderer = InApp->GetRenderer();

			/*ScopedGfxBind<GfxRenderBuffer> Binder;
			if (RenderBufferId.IsValid)
			{
				auto Buffer = GDI->GetRenderBuffer(RenderBufferId);
				Binder.Bind(Buffer);
			}*/

			u32 LastMatId = UINT_MAX;
			GfxShader* GfxShader = nullptr;

			for (u32 i = 0; i < CommandCount; ++i)
			{
				Key key = Keys[i];
				CommandPacket packet = Data[i];

				DrawKey64 DecodedKey = DecodeDrawKey64(key);

				if (DecodedKey.bIsValid && DecodedKey.MaterialId != LastMatId)
				{
					// note --- uhm we have no way to extract the generation for the material
					//			think about this - do we need the generation? is this ever going
					//			to change?! (gb, 10/17/19)
					ResourceId ResId;
					ResId.Generation = 1;
					ResId.Index = DecodedKey.MaterialId;
					ResId.IsValid = true;

					auto Material = Resources->FindMaterial(ResId);
					auto Shader = Resources->FindShader(Material->ShaderId);

					/*if (Renderer->CurrentShader == nullptr || Renderer->CurrentShader->GetId().Index != Shader->ResourceId.Index)
					{
						Renderer->CurrentShader = GDI->GetShader(Shader->ResourceId);
						//Renderer->CurrentShader->Bind();
					}*/

					//Renderer->CurrentShader->SetMat4("proj", Projection);
					//Renderer->CurrentShader->SetMat4("view", View);

					//Renderer->CurrentShader->SetVec4("material.albedo", Material->Albedo);
					//Renderer->CurrentShader->SetVec4("material.emissive", Material->Emissive);
					//Renderer->CurrentShader->SetFloat("material.metallic", Material->Metallic);
					//Renderer->CurrentShader->SetFloat("material.roughness", Material->Roughness);
					//Renderer->CurrentShader->SetFloat("material.ao", Material->Ao);

					// remove this again! this is only here as a test
					// but since we use a deferred renderer later this should really
					// be set by the lighting pass!
					LastMatId = DecodedKey.MaterialId;
				}

				do
				{
					const DrawDispatchFunction function = commandPacket::LoadBackendDispatchFunction(packet);
					const void* command = commandPacket::LoadCommand(packet);
					function(GDI.get(), Renderer.get(), command);

					packet = commandPacket::LoadNextCommandPacket(packet);
				} while (packet != nullptr);
			}
		}
	private:
		GfxRTBundle* RenderBundle;
		Key* Keys;
		CommandPacket* Data;
		std::atomic<u32> CommandCount;
		Allocators::LinearAllocator Allocator;
		Memory::HeapArea HeapMemory;


		mat4 Projection;
		mat4 View;
	};

}