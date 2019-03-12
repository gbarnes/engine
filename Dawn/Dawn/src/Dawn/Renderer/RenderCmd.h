#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

#include <type_traits>

typedef void(*BackendDispatchFunction)(const void*);


template<typename From, typename To>
union union_cast {
	From from;
	To   to;

	union_cast(From from)
		:from(from) { }

	To getTo() const { return to; }

	operator To () const
	{
		return to;
	}
};

namespace RenderCommand
{
	typedef void* Packet;

	static const size_t OFFSET_NEXT_COMMAND_PACKET = 0u;
	static const size_t OFFSET_BACKEND_DISPATCH_FUNCTION = OFFSET_NEXT_COMMAND_PACKET + sizeof(Packet);
	static const size_t OFFSET_COMMAND = OFFSET_BACKEND_DISPATCH_FUNCTION + sizeof(BackendDispatchFunction);

	template <typename T>
	Packet Create(size_t auxMemorySize)
	{
		return ::operator new(GetSize<T>(auxMemorySize));
	}

	template <typename T>
	size_t GetSize(size_t auxMemorySize)
	{
		return OFFSET_COMMAND + sizeof(T) + auxMemorySize;
	};

	Packet* GetNextCommandPacket(Packet packet)
	{
		return union_cast<Packet, Packet*>(reinterpret_cast<char*>(packet) + OFFSET_NEXT_COMMAND_PACKET);
	}

	template <typename T>
	Packet* GetNextCommandPacket(T* command)
	{
		return union_cast<T*, Packet*>(reinterpret_cast<char*>(command) - OFFSET_COMMAND + OFFSET_NEXT_COMMAND_PACKET);
	}

	BackendDispatchFunction* GetBackendDispatchFunction(Packet packet)
	{
		return union_cast<Packet, BackendDispatchFunction*>(reinterpret_cast<char*>(packet) + OFFSET_BACKEND_DISPATCH_FUNCTION);
	}

	template <typename T>
	T* GetCommand(Packet packet)
	{
		return union_cast<Packet, T*>(reinterpret_cast<char*>(packet) + OFFSET_COMMAND);
	}

	template <typename T>
	char* GetAuxiliaryMemory(T* command)
	{
		return reinterpret_cast<char*>(command) + sizeof(T);
	}

	void StoreNextCommandPacket(Packet packet, Packet nextPacket)
	{
		*RenderCommand::GetNextCommandPacket(packet) = nextPacket;
	}

	template <typename T>
	void StoreNextCommandPacket(T* command, Packet nextPacket)
	{
		*RenderCmd::GetNextCommandPacket<T>(command) = nextPacket;
	}

	void StoreBackendDispatchFunction(Packet packet, BackendDispatchFunction dispatchFunction)
	{
		*RenderCommand::GetBackendDispatchFunction(packet) = dispatchFunction;
	}

	const Packet LoadNextCommandPacket(const Packet packet)
	{
		return *GetNextCommandPacket(packet);
	}

	const BackendDispatchFunction LoadBackendDispatchFunction(const Packet packet)
	{
		return *GetBackendDispatchFunction(packet);
	}

	const void* LoadCommand(const Packet packet)
	{
		return reinterpret_cast<char*>(packet) + OFFSET_COMMAND;
	}
};

namespace backendDispatch
{
	void Draw(const void* data)
	{
		//const Draw* realData = union_cast<const void*, const Draw*>(data);
		//backend::Draw(realData->vertexCount, realData->startVertex);
	}
}

struct Draw
{
	static const BackendDispatchFunction DISPATCH_FUNCTION;
};
static_assert(std::is_pod<Draw>::value == true, "Draw must be a POD.");

const BackendDispatchFunction Draw::DISPATCH_FUNCTION = &backendDispatch::Draw;