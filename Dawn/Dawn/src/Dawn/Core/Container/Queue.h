#pragma once
#include "List.h"

namespace Dawn
{
	enum QueueType
	{
		QueueType_FiFo = 0,
		QueueType_LiFo = 0,
	};


	static const unsigned int MAX_QUEUE_SIZE = 4096u;
	static const unsigned int MASK_QUEUE = MAX_QUEUE_SIZE - 1u;

	// First In First Out Queue!
	template < class T >
	class Queue
	{
		template < class T >
		struct QueueNode
		{
			friend class CQueue;
		protected:
			T Data;
		};

	protected:
		QueueNode<T>* Nodes[MAX_QUEUE_SIZE] = { nullptr };
		long Top = 0;
		long Bottom = 0;

	public:

		T Pop() 
		{
			if (Top < Bottom)
			{
				QueueNode<T>* TempNode = this->Nodes[Top & MASK_QUEUE];
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
			QueueNode<T>* TempLastQueue = this->Nodes[Bottom & MASK_QUEUE];

			if (TempLastQueue != nullptr) 
			{
				OutputDebugStringW(L"Reached end of queue!");
				return;
			}

			QueueNode<T>* QueueNode = new QueueNode<T>();
			QueueNode->Data = Data;

			this->Nodes[Bottom & MASK_QUEUE] = QueueNode;
			Bottom++;
		}
	};
}