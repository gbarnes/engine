#pragma once
#include <Windows.h>
#include "List.h"

namespace Dawn
{
	enum CQueueType
	{
		CQueueType_FiFo = 0,
		CQueueType_LiFo = 0,
	};


	static const unsigned int MAX_QUEUE_SIZE = 4096u;
	static const unsigned int MASK_QUEUE = MAX_QUEUE_SIZE - 1u;

	// First In First Out Queue!
	template < class T >
	class CQueue
	{
		template < class T >
		struct CQueueNode
		{
			friend class CQueue;
		protected:
			T Data;
		};

	protected:
		CQueueNode<T>* Nodes[MAX_QUEUE_SIZE] = { nullptr };
		long Top = 0;
		long Bottom = 0;

	public:

		T Pop() 
		{
			if (Top < Bottom)
			{
				CQueueNode<T>* TempNode = this->Nodes[Top & MASK_QUEUE];
				T TempData = TempNode->Data;
				SafeDelete(TempNode);
				this->Nodes[Top & MASK_QUEUE] = nullptr;
				Top++;
				return TempData;
			}

			OutputDebugStringW(L"Reached end of queue!");
			return T();
		}

		void Push(T Data) 
		{
			CQueueNode<T>* TempLastQueue = this->Nodes[Bottom & MASK_QUEUE];

			if (TempLastQueue != nullptr) 
			{
				OutputDebugStringW(L"Reached end of queue!");
				return;
			}

			CQueueNode<T>* QueueNode = new CQueueNode<T>();
			QueueNode->Data = Data;

			this->Nodes[Bottom & MASK_QUEUE] = QueueNode;
			Bottom++;
		}
	};
}