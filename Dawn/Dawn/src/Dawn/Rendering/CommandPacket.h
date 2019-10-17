#pragma once
#include "DrawCallCommands.h"

namespace Dawn
{
	template<typename From, typename To>
	union union_cast {
		From from;
		To   to;

		union_cast(From from)
			:from(from) { }

		To getTo() const { return to; }
	};

	typedef void* CommandPacket;

	namespace commandPacket
	{
		static const size_t OFFSET_NEXT_COMMAND_PACKET = 0u;
		static const size_t OFFSET_BACKEND_DISPATCH_FUNCTION = OFFSET_NEXT_COMMAND_PACKET + sizeof(CommandPacket);
		static const size_t OFFSET_COMMAND = OFFSET_BACKEND_DISPATCH_FUNCTION + sizeof(DrawDispatchFunction);

		template <typename T>
		static CommandPacket Create(size_t auxMemorySize)
		{
			return ::operator new(GetSize<T>(auxMemorySize));
		}

		template <typename T>
		static size_t GetSize(size_t auxMemorySize)
		{
			return OFFSET_COMMAND + sizeof(T) + auxMemorySize;
		};

		static CommandPacket* GetNextCommandPacket(CommandPacket packet)
		{
			return union_cast<char*, CommandPacket*>(reinterpret_cast<char*>(packet) + OFFSET_NEXT_COMMAND_PACKET).getTo();
		}

		template <typename T>
		static CommandPacket* GetNextCommandPacket(T* command)
		{
			return union_cast<char*, CommandPacket*>(reinterpret_cast<char*>(command) - OFFSET_COMMAND + OFFSET_NEXT_COMMAND_PACKET).getTo();
		}

		static DrawDispatchFunction* GetBackendDispatchFunction(CommandPacket packet)
		{
			return union_cast<char*, DrawDispatchFunction*>(reinterpret_cast<char*>(packet) + OFFSET_BACKEND_DISPATCH_FUNCTION).getTo();
		}

		template <typename T>
		static T* GetCommand(CommandPacket packet)
		{
			return union_cast<char*, T*>(reinterpret_cast<char*>(packet) + OFFSET_COMMAND).getTo();
		}

		template <typename T>
		static char* GetAuxiliaryMemory(T* command)
		{
			return reinterpret_cast<char*>(command) + sizeof(T);
		}

		static void StoreNextCommandPacket(CommandPacket packet, CommandPacket nextPacket)
		{
			*commandPacket::GetNextCommandPacket(packet) = nextPacket;
		}

		template <typename T>
		static void StoreNextCommandPacket(T* command, CommandPacket nextPacket)
		{
			*commandPacket::GetNextCommandPacket<T>(command) = nextPacket;
		}

		static void StoreBackendDispatchFunction(CommandPacket packet, DrawDispatchFunction dispatchFunction)
		{
			*commandPacket::GetBackendDispatchFunction(packet) = dispatchFunction;
		}

		static const CommandPacket LoadNextCommandPacket(const CommandPacket packet)
		{
			return *GetNextCommandPacket(packet);
		}

		static const DrawDispatchFunction LoadBackendDispatchFunction(const  CommandPacket packet)
		{
			return *GetBackendDispatchFunction(packet);
		}

		static const void* LoadCommand(const CommandPacket packet)
		{
			return reinterpret_cast<char*>(packet) + OFFSET_COMMAND;
		}
	};
}