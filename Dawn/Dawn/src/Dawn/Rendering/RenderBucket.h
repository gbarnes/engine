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
#include "Core/Logging/Log.h"

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
			CommandCount = 0;

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

		void Allocate(i32 InMaxSize)
		{
			MaxSize = InMaxSize;
			if (Keys == nullptr && Data == nullptr && InMaxSize > 0)
			{
				Keys = new Key[InMaxSize];
				Data = new CommandPacket[InMaxSize];
			}
		}

		void Reset(const mat4& InViewMatrix, const mat4& InProjMatrix)
		{
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

			u32 LastMatId = UINT_MAX;

			if (Keys == nullptr || Data == nullptr) 
			{
				return;
			}

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

					auto currentPsoId = GDI->GetCurrentPSOId();
					if (!currentPsoId.IsValid || currentPsoId.Index != Material->PSOId.Index)
					{
						GDI->SetPipelineState(Material->PSOId);
						GDI->BindPipelineShaders();
						GDI->CommitShaderResources(Material->PSOId);

						CBMaterial material = {};
						material.Albedo = Material->Albedo;
						material.AO = Material->Ao;
						material.Emissive = Material->Emissive;
						material.Metallic = Material->Metallic;
						material.Roughness = Material->Roughness;
						GDI->UpdateConstantBuffer(CommonConstantBuffers::MaterialData, &material, sizeof(material));
					}

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

			GDI->ClearShaderResources();
		}
	private:
		Key* Keys;
		CommandPacket* Data;
		std::atomic<u32> CommandCount;
		Allocators::LinearAllocator Allocator;
		Memory::HeapArea HeapMemory;
		i32 MaxSize;

		mat4 Projection;
		mat4 View;
	};

}