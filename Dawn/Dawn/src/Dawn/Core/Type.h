#pragma once
#include <functional>
#include "inc_common.h"


namespace Dawn 
{
	class TypeMemberCollection;
	typedef std::function<void()> TypeInitMethod;

	class BaseType
	{
	public:
		virtual std::string GetName() { return ""; };
		virtual std::string ToString(void* InData) { return ""; };
	};

	class DoubleType : public BaseType
	{
		union {
			double* asDouble;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "double";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return std::to_string(*Data.asDouble);
		}
	};

	class FloatType : public BaseType
	{
		union {
			float* asFloat;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "float";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return std::to_string(*Data.asFloat);
		}
	};

	class I16Type : public BaseType
	{
		union {
			s16* asInt;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "s16";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return std::to_string(*Data.asInt);
		}
	};

	class I32Type : public BaseType
	{
		union {
			i32* asInt;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "i32";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return std::to_string(*Data.asInt);
		}
	};

	class I64Type : public BaseType
	{
		union {
			i64* asInt;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "i64";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return std::to_string(*Data.asInt);
		}
	};

	class CharType : public BaseType
	{
		union {
			c8* asChar;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "c8";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return std::to_string(*Data.asChar);
		}
	};

	class U16Type : public BaseType
	{
		union {
			u16* asInt;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "u16";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return std::to_string(*Data.asInt);
		}
	};

	class U32Type : public BaseType
	{
		union {
			u32* asInt;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "u32";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return std::to_string(*Data.asInt);
		}
	};

	class U64Type : public BaseType
	{
		union {
			u64* asInt;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "u64";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return std::to_string(*Data.asInt);
		}
	};

	class BoolType : public BaseType
	{
		union {
			bool* asBool;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "bool";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return std::to_string(*Data.asBool);
		}
	};

	class Vector2Type : public BaseType
	{
		union {
			vec2* asVec2;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "vec2";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return "{x: " + std::to_string(Data.asVec2->x) + ", y: " + std::to_string(Data.asVec2->y) + "}";
		}
	};

	class Vector3Type : public BaseType
	{
		union {
			vec3* asVec3;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "vec3";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return "{x: " + std::to_string(Data.asVec3->x) + ", y: " + std::to_string(Data.asVec3->y) + " z: " + std::to_string(Data.asVec3->z) + "}";
		}
	};

	class Vector4Type : public BaseType
	{
		union {
			vec4* asVec4;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "vec4";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return "{x: " + std::to_string(Data.asVec4->x) + ", y: " + std::to_string(Data.asVec4->y) + ", z: " + std::to_string(Data.asVec4->z) + ", w: " + std::to_string(Data.asVec4->w) + "}";
		}
	};

	class QuatType : public BaseType
	{
		union {
			quat* asQuat;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "quat";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return "{x: " + std::to_string(Data.asQuat->x) + ", y: " + std::to_string(Data.asQuat->y) + ", z: " + std::to_string(Data.asQuat->z) + ", w: " + std::to_string(Data.asQuat->w) + "}";
		}
	};

	class ColorType : public BaseType
	{
		union {
			vec4* asColor;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "vec4";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return "{r: " + std::to_string(Data.asColor->r) + ", g: " + std::to_string(Data.asColor->g) + ", b: " + std::to_string(Data.asColor->b) + ", a: " + std::to_string(Data.asColor->a) + "}";
		}
	};

	struct TypeMember
	{
		std::string Name;
		BaseType* Type;
		u64 Offset;
		bool bSerialize = true;
	};

	class DawnType 
	{
		friend class TypeMemberCollection;
	public:
		std::string Name;
		u64 Size;

		DawnType(std::string InName, u64 InSize, TypeInitMethod InInit)
			: Name(InName), Size(InSize) 
		{
			if(InInit != nullptr)
				InInit(); 

			if (Types == nullptr)
				Types = new std::map<std::string, DawnType*>();

			Types->emplace(std::make_pair(InName, this));
		}

		virtual ~DawnType() 
		{
			for (auto pair : Members)
				SafeDelete(pair.second.Type);

			SafeDelete(Types);
		};
		virtual std::string GetName() const { return Name; }
		virtual std::string ToString() const { return ""; }

		template<typename T>
		T* Get(void* InInstance, const std::string& s) 
		{
			auto member = Members[s];
			return (T*)((static_cast<char *>(InInstance) + member.Offset));
		}

		void* GetAsVoid(void* InInstance, const std::string& s)
		{
			auto member = Members[s];
			return (void*)((static_cast<char *>(InInstance) + member.Offset));
		}

		
		void AddMember(const std::string& InName, BaseType* InType, u64 InSize, bool bSerialize = true)
		{
			TypeMember member = { InName, InType, InSize, bSerialize };
			Members.emplace(std::make_pair(InName, member));
		}

		static DawnType* GetType(const std::string& InTypeName)
		{
			auto it = Types->find(InTypeName);
			if (it == Types->end())
				return nullptr;

			return it->second;
		}

		std::vector<TypeMember> GetMembers()
		{
			std::vector<TypeMember> retval;
			retval.reserve(this->Members.size());
			for (auto const& element : Members) {
				retval.push_back(element.second);
			}
			return retval;
		}
		

	private:
		static std::map<std::string, DawnType*>* Types;
		std::map<std::string, TypeMember> Members;
	};



#define REGISTER_TYPE(type)private:\
	static DawnType InnerType;\
	static void InitType();\
public:\
	static Dawn::DawnType* GetType();\


#define MAKE_TYPE_BEGIN(type)\
	Dawn::DawnType* type##::GetType() { return &InnerType; }\
	DawnType type##::InnerType = DawnType(#type, sizeof(type), std::bind(&##type##::InitType));\
	void type##::InitType() {\

#define ADD_MEMBER(classType, name, type, serialize) InnerType.AddMember(#name, new type(), offsetof(classType, name), serialize);\

#define MAKE_TYPE_END() }\

#define INIT_TYPE(type) type##::GetType();
};
