#pragma once



namespace Dawn
{
	template < class T >
	class List
	{
		template < class T >
		struct SListNode
		{
			T Payload;
			struct SListNode<T>* Next = nullptr;
			struct SListNode<T>* Previous = nullptr;
			//struct CEListNode* NextXORPrevious = nullptr;

			// TODO (gb): Implement XOR linked list later on once we got 
			// the normal list up and running!
			static struct SListNode<T>* XOR(SListNode<T>* Head, SListNode<T>* Next)
			{
				return ((SListNode<T>*)((unsigned int)(Head) ^ (unsigned int)(Next)));
			}
		};

	public:
		class Iterator 
		{
			friend class List;

		protected:
			SListNode<T> * CurrentNode = nullptr;
			List<T>* InternalList;

		public:
			bool HasNext() const
			{
				return (CurrentNode != nullptr);
			}

			bool HasPrevious() const
			{
				return HasNext();
			}

			T& Next() 
			{
				SListNode<T> * CurrentTemp = this->CurrentNode;
				this->CurrentNode = CurrentTemp->Next;
				return CurrentTemp->Payload;
			}

			T& Previous()
			{
				SListNode<T> * CurrentTemp = this->CurrentNode;
				this->CurrentNode = CurrentTemp->Previous;
				return CurrentTemp->Payload;
			}

			T Remove()
			{
				if (CurrentNode == nullptr)
					return T();

				T TempPayload = CurrentNode->Payload;
				SListNode<T> * CurrentTemp = this->CurrentNode;

				SListNode<T> * PreviousTemp = CurrentTemp->Previous;
				SListNode<T> * NextTemp = CurrentTemp->Next;

				// We need to take care of connecting 
				// the next node with the previous of the 
				// current one. If the previous node is null
				// we know that the next node has to be 
				// the new head!
				if (PreviousTemp != nullptr)
					PreviousTemp->Next = NextTemp;
				else
					InternalList->Head = NextTemp;

				// We need to take care of connecting 
				// the next node with the previous of the 
				// current one. If the next node is null
				// we know that the previous node has to be 
				// the new tail!
				if (NextTemp != nullptr)
					NextTemp->Previous = PreviousTemp;
				else
					InternalList->Tail = PreviousTemp;


				this->CurrentNode = NextTemp;
				SafeDelete(CurrentTemp);
				return TempPayload;
			}

		public:
			Iterator operator*(SListNode<T>* Node)
			{
				this->CurrentNode = Node;
			}
		};

		friend class Iterator;
	protected:
		struct SListNode<T> * Head = nullptr;
		struct SListNode<T> * Tail = nullptr;

	public:
		~List()
		{
			Clear();
		}

		// Clears the entire list and its nodes!
		void Clear() 
		{
			SListNode<T>* Node = Tail;
			while (Node != nullptr)
			{
				SListNode<T>* temp = Node;
				Node = Node->Previous;
				SafeDelete(temp);
			}

			Tail = nullptr;
			Head = nullptr;
		}

		// Pushes a new node with the given
		// data to the front of the list.
		void PushFront(T data)
		{
			SListNode<T>* NewNode = new SListNode<T>();
			NewNode->Payload = data;
			
			if (Head != nullptr) 
			{
				NewNode->Next = Head;
				Head->Previous = NewNode;
			}

			// we want to add the new element to the tail aswell
			// in case its the first ever element in the list!
			if (Tail == nullptr && Head == nullptr) 
				Tail = NewNode;

			Head = NewNode;
		}

		// Removes the head node of the list
		T PopFront()
		{
			if (Head != nullptr)
			{
				SListNode<T>* TempHead = Head;

				// since we are clearing out the current head we need 
				// to seed the Previous of the head's next node to null
				SListNode<T>* TempNext = Head->Next;
				if (TempNext != nullptr)
				{
					TempNext->Previous = nullptr;
					Head = TempNext;
				}
				else
				{
					// in case we have no previous node 
					// we know that the current tail node is 
					// the last within the list so we need to clear
					// both Tail and Head!
					Tail = nullptr;
					Head = nullptr;
				}

				T TempData = TempHead->Payload;

				// finally free some memory!
				SafeDelete(TempHead);

				return TempData;
			}

			return T();
		}

		// Pushes a new node to the front of the list!
		void PushBack(T data)
		{
			SListNode<T>* NewNode = new SListNode<T>();
			NewNode->Payload = data;

			if (Head == nullptr && Tail == nullptr)
				Head = NewNode;

			if (Tail != nullptr) 
			{
				NewNode->Previous = Tail;
				Tail->Next = NewNode;
			}

			Tail = NewNode;
		}

		// Removes the head node of the list
		T PopBack()
		{
			if (Tail != nullptr) 
			{
				SListNode<T>* TempTail = Tail;

				// since we are clearing out the current tail we need 
				// to seed the Previous of the tails's previous node to null
				SListNode<T>* TempPrevious = Tail->Previous;
				if (TempPrevious != nullptr)
				{
					TempPrevious->Next = nullptr;
					Tail = TempPrevious;
				}
				else
				{
					// in case we have no next node 
					// we know that the current head node is 
					// the last within the list so we need to clear
					// both Tail and Head!
					Tail = nullptr;
					Head = nullptr;
				}

				T TempData = TempTail->Payload;

				// finally free some memory!
				SafeDelete(TempTail);

				return TempData;
			}

			return T();
		}

		T Remove(u32 Index)
		{
			if (Index >= this->Count())
				return T();

			Iterator It = this->Begin();
			u32 Counter = 0;
			while (It.HasNext())
			{
				if (Index == Counter)
				{
					return It.Remove();
				}

				It.Next();
				Counter++;
			}

			return T();
		}

		u32 Count() const
		{
			u32 nodesInList = 0;
			SListNode<T>* Next = Head;

			while (Next != nullptr)
			{
				nodesInList++;
				Next = Next->Next;
			}

			return nodesInList;
		}

		Iterator Begin() 
		{
			Iterator It;
			It.InternalList = this;
			It.CurrentNode = this->Head;
			return It;
		}

		Iterator End()
		{
			Iterator It;
			It.InternalList = this;
			It.CurrentNode = this->Tail;
			return It;
		}
	};
}

