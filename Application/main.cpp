#include <iostream>
#include "List.h"
#include "Array.h"
#include "Map.h"

using namespace DS;

int main() 
{
	CList<int> List;
	List.PushBack(1);
	List.PushBack(2);
	List.PushFront(0);
	List.PopBack();
	List.PushBack(3);
	List.PopFront();
	List.Remove(0);

	auto It = List.Begin();
	while (It.HasNext())
	{
		int& i = It.Next();
		std::cout << i << "\n";
	}

	//List.Clear();

	CArray<int> Stuff;
	Stuff.Push(1); // 0
	Stuff.Push(2); // 1
	Stuff.Push(3); // 2
	Stuff.Push(4); // 3
	Stuff.Remove(4);
	//Stuff.FindIndex(1);

	for (int i = 0; i < Stuff.Count(); i++)
	{
		std::cout << "Value at Index " << i << ": " << Stuff[i] << "\n";
	}

	Stuff.Clear();

	CMap<std::string, int> TestMap;
	TestMap.Push("Test", 1);
	TestMap.Push("Test2", 3);
	TestMap.Push("Test4", 5);
	int val = TestMap["Test"];
	int val2 = TestMap["Test4"];

	TestMap.Clear();

	_CrtDumpMemoryLeaks();
	system("pause");

}