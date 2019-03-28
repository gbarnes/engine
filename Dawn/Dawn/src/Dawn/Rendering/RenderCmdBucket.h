#pragma once
typedef void(*BackendDispatchFunction)(const void*);

template<typename T>
class RenderCmdBucket
{
	typedef T Key;
public:
	template<typename T>
	void RenderCmdBucket::Submit()
	{
		for (unsigned int i = 0; i < CommandCount; ++i)
		{
			Key key = Keys[i];
			void* data = Data[i];
		}
	}

	template<typename U>
	U* RenderCmdBucket::AddCommand(Key InKey, BackendDispatchFunction InMethod)
	{
		U* Data = AllocateCommand<U>();

		Add(InKey, Data, InMethod);

		return Data;
	}
private:
	unsigned int CommandCount;
	Key* Keys;
	void** Data;
	BackendDispatchFunction* Functions;

	template<typename U>
	void RenderCmdBucket::Add(Key InKey, U* InData, BackendDispatchFunction InFunction)
	{
		Data[CommandCount - 1] = InData;
		Keys[CommandCount - 1] = InKey;
		Functions[CommandCount - 1] = InFunction;
	}
};




