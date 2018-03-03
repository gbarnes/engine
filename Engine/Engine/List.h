#pragma once

#include <memory>
#include <iostream>
#include <string>
#include <cstdio>
#include "inc_common.h"
#include "Core\System\ComPtr.h"


namespace DS 
{
	template < class T >
	struct CListNode
	{
		T Payload;
		struct CListNode<T>* Next = nullptr;
		struct CListNode<T>* Previous = nullptr;
		//struct CEListNode* NextXORPrevious = nullptr;

		// TODO (gb): Implement XOR linked list later on once we got 
		// the normal list up and running!
		static struct CListNode<T>* XOR(CListNode<T>* Head, CListNode<T>* Next)
		{
			return ((CListNode<T>*)((unsigned int)(Head) ^ (unsigned int)(Next)));
		}
	};

	template < class T >
	class CList
	{
		
	public:
		class Iterator 
		{
			friend class CList;

		protected:
			CListNode<T> * CurrentNode = nullptr;

		public:
			bool HasNext() const
			{
				return (CurrentNode != nullptr);
			}

			bool HasPrevious() const
			{
				return HasNext();
			}

			T Next() 
			{
				CListNode<T> * CurrentTemp = this->CurrentNode;
				this->CurrentNode = CurrentTemp->Next;
				return CurrentTemp->Payload;
			}

			T Previous()
			{
				CListNode<T> * CurrentTemp = this->CurrentNode;
				this->CurrentNode = CurrentTemp->Previous;
				return CurrentTemp->Payload;
			}

		public:
			Iterator operator*(CListNode<T>* Node)
			{
				this->CurrentNode = Node;
			}
		};

	protected:
		struct CListNode<T> * Head = nullptr;
		struct CListNode<T> * Tail = nullptr;

	public:
		~CList()
		{
			Clear();
		}

		// Clears the entire list and its nodes!
		void Clear() 
		{
			CListNode<T>* Node = Tail;
			while (Node != nullptr)
			{
				CListNode<T>* temp = Node;
				Node = Node->Previous;
				delete temp;
			}

			Tail = nullptr;
			Head = nullptr;
		}

		// Pushes a new node with the given
		// data to the front of the list.
		void PushFront(T data)
		{
			CListNode<T>* NewNode = new CListNode<T>();
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
				CListNode<T>* TempHead = Head;

				// since we are clearing out the current head we need 
				// to seed the Previous of the head's next node to null
				CListNode<T>* TempNext = Head->Next;
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
				delete TempHead;

				return TempData;
			}

			return T();
		}

		// Pushes a new node to the front of the list!
		void PushBack(T data)
		{
			CListNode<T>* NewNode = new CListNode<T>();
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
				CListNode<T>* TempTail = Tail;

				// since we are clearing out the current tail we need 
				// to seed the Previous of the tails's previous node to null
				CListNode<T>* TempPrevious = Tail->Previous;
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
				delete TempTail;

				return TempData;
			}

			return T();
		}

		u32 Count() const
		{
			u32 nodesInList = 0;
			CListNode<T>* Next = Head;

			while (Next != nullptr)
			{
				nodesInList++;
				Next = Next->Next;
			}

			return nodesInList;
		}

		Iterator Begin() const 
		{
			Iterator it;
			it.CurrentNode = this->Head;
			return it;
		}

		Iterator End() const
		{
			Iterator it;
			it.CurrentNode = this->Tail;
			return it;
		}
	};
}

