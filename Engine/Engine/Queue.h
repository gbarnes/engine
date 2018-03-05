#pragma once
#include "List.h"

namespace DS
{
	enum CQueueType
	{
		CQueueType_FiFo = 0,
		CQueueType_LiFo = 0,
	};


	template < class T >
	class CQueue : CList<T>
	{
		template < class T >
		struct CQueueNode
		{
		protected:
			T Data;
			CQueueNode<T>* Next;
		};

	protected:
		CQueueNode<T>* Begin;
		CQueueNode<T>* End;
	};
}