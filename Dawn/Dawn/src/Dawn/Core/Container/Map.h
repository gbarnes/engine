#pragma once
#include "../../inc_common.h"
#include "Array.h"
#include "List.h"

namespace Dawn
{
#define MAP_TABLE_SIZE 20

	template < class K, class V>
	class CKeyValuePair
	{
	public:
	
		CKeyValuePair(K Key, V Value)
		{
			this->InternalKey = Key;
			this->InternalValue = Value;
		}


		K Key() 
		{
			return this->InternalKey;
		}

		V Value() 
		{
			return this->InternalValue;
		}

		void SetValue(V Value)
		{
			this->InternalValue = Value;
		}

	private:
		// TODO (gb): use better naming here! Just for testing atm.
		K InternalKey;
		V InternalValue;
	};

	template < class K, class V, class Hash = std::hash<K>>
	class CMap
	{
	private:
		//CMapNode<K, V>** Nodes;
		Dawn::CList< CKeyValuePair<K, V>* > Nodes[MAP_TABLE_SIZE];

	public:
		void Push(K Key, V Value) 
		{
			Hash KeyHashed;
			i64 Index = KeyHashed(Key) % MAP_TABLE_SIZE;
			Dawn::CList<CKeyValuePair<K, V>*>* Bucket = &this->Nodes[Index];
			if (Bucket->Count() > 0)
			{
				Dawn::CList<CKeyValuePair<K, V>*>::Iterator It = Bucket->Begin();
				while (It.HasNext())
				{
					CKeyValuePair<K, V>* ValuePair = It.Next();
					if (ValuePair->Key() == Key)
					{
						ValuePair->SetValue(Value);
						return;
					}
				}
			}

			Bucket->PushBack(new CKeyValuePair<K, V>(Key, Value));
		}

		V Remove(K Key)
		{
			Hash KeyHashed;
			i64 Index = KeyHashed(Key) % MAP_TABLE_SIZE;
			Dawn::CList<CKeyValuePair<K, V>*>* Bucket = &this->Nodes[Index];
			if (Bucket->Count() > 0)
			{
				Dawn::CList<CKeyValuePair<K, V>*>::Iterator It = Bucket->Begin();
				while (It.HasNext())
				{
					CKeyValuePair<K, V>* ValuePair = It.Next();
					if (ValuePair->Key() == Key)
					{
						V TempValue = ValuePair->Value();
						It.Remove();
						delete ValuePair;
						return TempValue;
					}
				}
			}

			return V();
		}

		bool Exists(K Key) 
		{
			Hash KeyHashed;
			i64 Index = KeyHashed(Key) % MAP_TABLE_SIZE;
			Dawn::CList<CKeyValuePair<K, V>*>* Bucket = &this->Nodes[Index];
			return Bucket->Count() > 0;
		}

		void Clear()
		{
			for (int i = 0; i < MAP_TABLE_SIZE; i++)
			{
				Dawn::CList<CKeyValuePair<K, V>*>* Bucket = &this->Nodes[i];
				while (Bucket->Count() > 0)
				{
					delete Bucket->PopFront();
				}
			}
		}

	public:
		// 
		V operator[](K Key) 
		{
			CKeyValuePair<K, V>* KeyValuePair = FindKeyValuePair(Key);
			if (KeyValuePair)
				return KeyValuePair->Value();

			return V();
		}

	protected:

		//
		CKeyValuePair<K, V>* FindKeyValuePair(K Key)
		{
			Hash KeyHashed;
			i64 Index = KeyHashed(Key) % MAP_TABLE_SIZE;
			Dawn::CList<CKeyValuePair<K, V>*>* Bucket = &this->Nodes[Index];
			Dawn::CList<CKeyValuePair<K, V>*>::Iterator It = Bucket->Begin();

			while (It.HasNext())
			{
				CKeyValuePair<K, V>* Node = It.Next();
				if (Node->Key() == Key)
					return Node;
			}

			return nullptr;
		}
	};

}