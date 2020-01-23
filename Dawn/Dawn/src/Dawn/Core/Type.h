#pragma once

namespace Dawn
{
	class TypeMemberCollection;
	typedef std::function<void()> TypeInitMethod;

	class BaseType
	{
	public:
		virtual std::string GetName() { return ""; };
		virtual std::string ToString(void* InData) { return ""; };
		virtual void FromString(void* InData, const std::string& InSerializedData) {};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			*Data.asDouble = std::stod(InSerializedData);
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			*Data.asFloat = std::stof(InSerializedData);
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			*Data.asInt = std::stoi(InSerializedData);
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			*Data.asInt = std::stoi(InSerializedData);
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			*Data.asInt = std::stoll(InSerializedData);
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			*Data.asChar = std::stoi(InSerializedData);
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			*Data.asInt = (u16)std::stoul(InSerializedData);
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			*Data.asInt = (u32)std::stoul(InSerializedData);
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			*Data.asInt = std::stoull(InSerializedData);
		};
	};

	class HandleType : public BaseType
	{
		union {
			GenericHandle* asHandle;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "GenericHandle";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return "{Index: " + std::to_string(Data.asHandle->Index) + ", Generation: " + std::to_string(Data.asHandle->Generation) + "}";
		}

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;

			std::regex filter("\\{Index: ([-]?[0-9]*.[0-9]*), Generation: ([-]?[0-9]*.[0-9]*)\\}");
			std::smatch matches;
			if (std::regex_search(InSerializedData, matches, filter))
			{
				Data.asHandle->Index = std::stoul(matches.str(1));
				Data.asHandle->Generation = std::stoul(matches.str(2));
			}
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			*Data.asBool = (std::stoi(InSerializedData) == 0) ? false : true;
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;

			std::regex filter("\\{x: ([-]?[0-9]*.[0-9]*), y: ([-]?[0-9]*.[0-9]*)\\}");
			std::smatch matches;
			if (std::regex_search(InSerializedData, matches, filter))
			{
				Data.asVec2->x = std::stof(matches.str(1));
				Data.asVec2->y = std::stof(matches.str(2));
			}
		};
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
			return "{x: " + std::to_string(Data.asVec3->x) + ", y: " + std::to_string(Data.asVec3->y) + ", z: " + std::to_string(Data.asVec3->z) + "}";
		}

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;

			std::regex filter("\\{x: ([-]?[0-9]*.[0-9]*), y: ([-]?[0-9]*.[0-9]*), z: ([-]?[0-9]*.[0-9]*)\\}");
			std::smatch matches;
			if (std::regex_search(InSerializedData, matches, filter))
			{
				Data.asVec3->x = std::stof(matches.str(1));
				Data.asVec3->y = std::stof(matches.str(2));
				Data.asVec3->z = std::stof(matches.str(3));
			}
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;

			std::regex filter("\\{x: ([-]?[0-9]*.[0-9]*), y: ([-]?[0-9]*.[0-9]*), z: ([-]?[0-9]*.[0-9]*), w: ([-]?[0-9]*.[0-9]*)\\}");
			std::smatch matches;
			if (std::regex_search(InSerializedData, matches, filter))
			{
				Data.asVec4->x = std::stof(matches.str(1));
				Data.asVec4->y = std::stof(matches.str(2));
				Data.asVec4->z = std::stof(matches.str(3));
				Data.asVec4->w = std::stof(matches.str(4));
			}
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;

			std::regex filter("\\{x: ([-]?[0-9]*.[0-9]*), y: ([-]?[0-9]*.[0-9]*), z: ([-]?[0-9]*.[0-9]*), w: ([-]?[0-9]*.[0-9]*)\\}");
			std::smatch matches;
			if (std::regex_search(InSerializedData, matches, filter))
			{
				Data.asQuat->x = std::stof(matches.str(1));
				Data.asQuat->y = std::stof(matches.str(2));
				Data.asQuat->z = std::stof(matches.str(3));
				Data.asQuat->w = std::stof(matches.str(4));
			}
		};
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

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			std::regex filter("\\{r: ([-]?[0-9]*.[0-9]*), g: ([-]?[0-9]*.[0-9]*), b: ([-]?[0-9]*.[0-9]*), a: ([-]?[0-9]*.[0-9]*)\\}");
			std::smatch matches;
			if (std::regex_search(InSerializedData, matches, filter))
			{
				Data.asColor->x = std::stof(matches.str(1));
				Data.asColor->y = std::stof(matches.str(2));
				Data.asColor->z = std::stof(matches.str(3));
				Data.asColor->a = std::stof(matches.str(4));
			}
		};
	};

	class AABBoxType : public BaseType
	{
		union {
			AABBox* asBox;
			void* asVoid;
		} Data;

	public:
		std::string GetName() override
		{
			return "AABBox";
		}

		std::string ToString(void* InData) override
		{
			Data.asVoid = InData;
			return "";
		}

		void FromString(void* InData, const std::string& InSerializedData) override
		{
			Data.asVoid = InData;
			
		};
	};

	struct TypeMember
	{
		std::string Name;
		BaseType* Type;
		u64 Offset;
		bool bSerialize = true;
	};

	typedef std::function<void*()> InstanceCreateDelegate;

	class DawnType
	{
		friend class TypeMemberCollection;
	public:
		std::string Name;
		u64 Size;

		DawnType(std::string InName, u64 InSize, TypeInitMethod InInit)
			: Name(InName), Size(InSize)
		{
			if (InInit != nullptr)
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
			auto member = &Members[s];
			return (void*)((static_cast<char *>(InInstance) + member->Offset));
		}

		void DeserializeMember(void* InInstance, const std::string& InName, const std::string& InValue)
		{
			auto member = &Members[InName];
			member->Type->FromString((void*)((static_cast<char *>(InInstance) + member->Offset)), InValue);
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

		void AssignCreationDelegate(InstanceCreateDelegate InDelegate)
		{
			Delegate = InDelegate;
		}

		void* CreateInstance()
		{
			if (!Delegate)
				return nullptr;

			return Delegate();
		}

	private:

		static std::map<std::string, DawnType*>* Types;
		InstanceCreateDelegate Delegate;
		std::map<std::string, TypeMember> Members;
	};
	
#define REGISTER_TYPE(type)private:\
	static DawnType InnerType;\
	static void InitType();\
	public:\
	static Dawn::DawnType* GetType();\
	static void* CreateInstance();\


#define MAKE_TYPE_BEGIN(type)\
	Dawn::DawnType* type##::GetType() { return &InnerType; }\
	void* type##::CreateInstance() { return new type##(); }\
	Dawn::DawnType type##::InnerType = Dawn::DawnType(#type, sizeof(type), std::bind(&##type##::InitType));\
	void type##::InitType() {\
		InnerType.AssignCreationDelegate(std::bind(&##type##::CreateInstance));\

#define ADD_MEMBER(classType, name, type, serialize) InnerType.AddMember(#name, new type(), offsetof(classType, name), serialize);\

#define MAKE_TYPE_END() }\

#define INIT_TYPE(type) type##::GetType();\

};