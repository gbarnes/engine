#pragma once


struct HierarchyNode
{
	i32 DataIndex;
	HierarchyNode* Parent;
	std::vector<HierarchyNode*> Childs;
};

template<typename T>
struct HierarchyNodeData
{
	T	Data;
	i64 NodeRef;
};

template<typename T>
class HierarchyGraph
{
public:
	HierarchyNode* CreateNode() { return new HierarchyNode(); }

	HierarchyNode* AddNode(T InData, HierarchyNode* InParent = nullptr)
	{
		auto* Node = CreateNode();

		HierarchyNodeData<T> DataNode;
		DataNode.Data = InData;
		DataNode.NodeRef = reinterpret_cast<i64>(Node);

		Data.push_back(DataNode);

		Node->Parent = InParent;
		Node->DataIndex = static_cast<i32>(Data.size() - 1);
		
		if (InParent)
			InParent->Childs.push_back(Node);
		else
			Roots.push_back(Node);

		return Node;
	}

	//
	// Moves the Node attached to the Data Index to the 
	// new parent. If -1 is set the Node will have no parent.
	//
	void SetParent(i32 InDataIndex, i32 InDataParent = -1)
	{
		HierarchyNodeData<T>& DataObj = Data[InDataIndex];
		
		HierarchyNode* Node = ((HierarchyNode*)DataObj.NodeRef);
		if (Node->Parent != nullptr)
		{
			auto it = std::find(Node->Parent->Childs.begin(), Node->Parent->Childs.end(), Node);
			if (it != Node->Parent->Childs.end()) Node->Parent->Childs.erase(it);
		}
		else
		{
			auto it = std::find(Roots.begin(), Roots.end(), Node);
			if (it != Roots.end()) Roots.erase(it);
		}

		if (InDataParent == -1)
		{
			Node->Parent = nullptr;
			Roots.push_back(Node);
		}
		else 
		{
			Node->Parent = (HierarchyNode*)((HierarchyNodeData<T>&)Data[InDataParent]).NodeRef;
			Node->Parent->Childs.push_back(Node);
		}
	}

	T* GetDataFromNode(HierarchyNode* InNode)
	{
		return &this->Data[InNode->DataIndex].Data;
	}

	T* GetDataFromIndex(i32 InIndex)
	{
		return &this->Data[InIndex].Data;
	}

	T* GetParentDataFromIndex(i32 InIndex)
	{
		auto* Node = (HierarchyNode*)this->Data[InIndex].NodeRef;

		return (Node->Parent == nullptr) ? nullptr : &this->Data[Node->DataIndex].Data;
	}

	const std::vector<HierarchyNode*>& GetRoots() const {
		return Roots;
	}

private:
	std::vector<HierarchyNodeData<T>> Data;
	std::vector<HierarchyNode*> Roots;
};