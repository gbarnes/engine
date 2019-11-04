#pragma once
#include <functional>
#include "inc_common.h"


namespace Dawn 
{
	class TypeMemberCollection;
	typedef std::function<void()> TypeInitMethod;


	class Type 
	{
		friend class TypeMemberCollection;
	public:
		std::string Name;
		u64 Size;

		Type(std::string InName, u64 InSize, TypeInitMethod InInit)
			: Name(InName), Size(InSize) 
		{
			if(InInit != nullptr)
				InInit(); 

			if (Types == nullptr)
				Types = new std::map<std::string, Type*>();

			Types->emplace(std::make_pair(InName, this));
		}

		virtual ~Type() 
		{
			SafeDelete(Types);
		};
		virtual std::string GetName() const { return Name; }
		virtual std::string ToString() const { return ""; }

		template<typename T>
		T* Get(void* InInstance, std::string s) 
		{
			auto member = Members[s];
			return (T*)((static_cast<char *>(InInstance) + member.Offset));
		}

		
		void AddMember(std::string InName, u64 InSize) 
		{
			TypeMember member = { InName, InSize };
			Members.emplace(std::make_pair(InName, member));
			
		}

		static Type* GetType(const std::string& InTypeName)
		{
			auto it = Types->find(InTypeName);
			if (it == Types->end())
				return nullptr;

		

			return it->second;
		}

		

	private:
		static std::map<std::string, Type*>* Types;

		struct TypeMember
		{
			std::string Name;
			u64 Offset;
			bool bSerialize = true;
		};

		std::map<std::string, TypeMember> Members;
	};



#define REGISTER_TYPE(type)private:\
	static Type InnerType;\
	static void InitType();\
public:\
	static Dawn::Type* GetType();\


#define MAKE_TYPE_BEGIN(type)\
	Dawn::Type* type##::GetType() { return &InnerType; }\
	Type type##::InnerType = Type(#type, sizeof(type), std::bind(&##type##::InitType));\
	void type##::InitType() {\

#define ADD_MEMBER(classType, name, type) InnerType.AddMember(#name, offsetof(classType, name));\

#define MAKE_TYPE_END() }\

#define INIT_TYPE(type) type##::GetType();
};
