#pragma once
#include "generic.h"
#include <filesystem>

namespace fs = std::filesystem;

class File {
private:
	FILE* file;
public:
	File(fs::path path, const char* mode) { fopen_s(&file, path.string().c_str(), mode); }
	~File() { if (file) { fclose(file); } }
	operator bool() const { return file != nullptr; }
	operator FILE* () { return file; }
};

// Wrapper for array unit in global names array
class UE_FNameEntry {
protected:
	uint8* object;
public:
	UE_FNameEntry(uint8* object) : object(object) {}
	UE_FNameEntry() : object(nullptr) {}

	// Gets string out of array unit
	std::string String() const;

	// Gets string out of array unit
	void String(char* buf, uint16 len) const;
	operator bool() const { return object != nullptr; }
};

class UE_FName {
protected:
	uint8* object;
	bool enc;
public:
	UE_FName(uint8* object, bool enc) : object(object), enc(enc) {}
	UE_FName() : object(nullptr), enc(false) {}

	std::string GetName() const;
};

class UE_UClass;

class UE_UObject
{
protected:
	uint8* object;
public:
	UE_UObject(uint8* object) : object(object) {}
	UE_UObject() : object(nullptr) {}
	bool operator==(const UE_UObject& obj) const { return obj.object == object; };
	bool operator!=(const UE_UObject& obj) const { return obj.object != object; };
	uint32 GetIndex() const;
	UE_UClass GetClass() const;
	UE_UObject GetOuter() const;
	UE_UObject GetPackageObject() const;
	std::string GetName() const;
	std::string GetFullName() const;
	std::string GetCppName() const;
	void* GetAddress() const { return object; }
	operator uint8* () const { return object; };
	operator bool() const { return object != nullptr; }

	template<typename Base>
	Base Cast() const { return Base(object); }

	template<typename T>
	bool IsA() const;

	static UE_UClass StaticClass();
};

class UE_AActor : public UE_UObject
{
public:
	static UE_UClass StaticClass();
};

class UE_UField : public UE_UObject
{
public:
	using UE_UObject::UE_UObject;
	UE_UField GetNext() const;
	static UE_UClass StaticClass();
};

class UE_UStruct : public UE_UField
{
public:
	using UE_UField::UE_UField;

	UE_UStruct GetSuper() const;
	UE_UField GetChildren() const;
	int32 GetSize() const;
	static UE_UClass StaticClass();
};

enum EFunctionFlags : uint32
{
	// Function flags.
	FUNC_None = 0x00000000,

	FUNC_Final = 0x00000001,	// Function is final (prebindable, non-overridable function).
	FUNC_RequiredAPI = 0x00000002,	// Indicates this function is DLL exported/imported.
	FUNC_BlueprintAuthorityOnly = 0x00000004,   // Function will only run if the object has network authority
	FUNC_BlueprintCosmetic = 0x00000008,   // Function is cosmetic in nature and should not be invoked on dedicated servers
	// FUNC_				= 0x00000010,   // unused.
	// FUNC_				= 0x00000020,   // unused.
	FUNC_Net = 0x00000040,   // Function is network-replicated.
	FUNC_NetReliable = 0x00000080,   // Function should be sent reliably on the network.
	FUNC_NetRequest = 0x00000100,	// Function is sent to a net service
	FUNC_Exec = 0x00000200,	// Executable from command line.
	FUNC_Native = 0x00000400,	// Native function.
	FUNC_Event = 0x00000800,   // Event function.
	FUNC_NetResponse = 0x00001000,   // Function response from a net service
	FUNC_Static = 0x00002000,   // Static function.
	FUNC_NetMulticast = 0x00004000,	// Function is networked multicast Server -> All Clients
	FUNC_UbergraphFunction = 0x00008000,   // Function is used as the merge 'ubergraph' for a blueprint, only assigned when using the persistent 'ubergraph' frame
	FUNC_MulticastDelegate = 0x00010000,	// Function is a multi-cast delegate signature (also requires FUNC_Delegate to be set!)
	FUNC_Public = 0x00020000,	// Function is accessible in all classes (if overridden, parameters must remain unchanged).
	FUNC_Private = 0x00040000,	// Function is accessible only in the class it is defined in (cannot be overridden, but function name may be reused in subclasses.  IOW: if overridden, parameters don't need to match, and Super.Func() cannot be accessed since it's private.)
	FUNC_Protected = 0x00080000,	// Function is accessible only in the class it is defined in and subclasses (if overridden, parameters much remain unchanged).
	FUNC_Delegate = 0x00100000,	// Function is delegate signature (either single-cast or multi-cast, depending on whether FUNC_MulticastDelegate is set.)
	FUNC_NetServer = 0x00200000,	// Function is executed on servers (set by replication code if passes check)
	FUNC_HasOutParms = 0x00400000,	// function has out (pass by reference) parameters
	FUNC_HasDefaults = 0x00800000,	// function has structs that contain defaults
	FUNC_NetClient = 0x01000000,	// function is executed on clients
	FUNC_DLLImport = 0x02000000,	// function is imported from a DLL
	FUNC_BlueprintCallable = 0x04000000,	// function can be called from blueprint code
	FUNC_BlueprintEvent = 0x08000000,	// function can be overridden/implemented from a blueprint
	FUNC_BlueprintPure = 0x10000000,	// function can be called from blueprint code, and is also pure (produces no side effects). If you set this, you should set FUNC_BlueprintCallable as well.
	FUNC_EditorOnly = 0x20000000,	// function can only be called from an editor scrippt.
	FUNC_Const = 0x40000000,	// function can be called from blueprint code, and only reads state (never writes state)
	FUNC_NetValidate = 0x80000000,	// function must supply a _Validate implementation

	FUNC_AllFlags = 0xFFFFFFFF,
};

class UE_UFunction : public UE_UStruct
{
public:
	using UE_UStruct::UE_UStruct;
	uint64 GetFunc() const;
	std::string GetFunctionFlags() const;
	static UE_UClass StaticClass();
};

class UE_UScriptStruct : public UE_UStruct
{
public:
	using UE_UStruct::UE_UStruct;
	static UE_UClass StaticClass();
};

class UE_UClass : public UE_UStruct {
public:
	using UE_UStruct::UE_UStruct;
	static UE_UClass StaticClass();
};

class UE_UEnum : public UE_UField
{
public:
	using UE_UField::UE_UField;
	TArray GetNames() const;
	static UE_UClass StaticClass();
};

enum class PropertyType {
	Unknown,
	StructProperty,
	ObjectProperty,
	SoftObjectProperty,
	FloatProperty,
	ByteProperty,
	BoolProperty,
	IntProperty,
	Int8Property,
	Int16Property,
	Int64Property,
	UInt16Property,
	UInt32Property,
	UInt64Property,
	NameProperty,
	DelegateProperty,
	SetProperty,
	ArrayProperty,
	WeakObjectProperty,
	StrProperty,
	TextProperty,
	MulticastSparseDelegateProperty,
	EnumProperty,
	DoubleProperty,
	MulticastDelegateProperty,
	ClassProperty,
	MulticastInlineDelegateProperty,
	MapProperty,
	InterfaceProperty
};

class UE_UProperty : public UE_UField
{
public:
	using UE_UField::UE_UField;

	int32 GetArrayDim() const;
	int32 GetSize() const;
	int32 GetOffset() const;
	uint64 GetPropertyFlags() const;
	std::pair<PropertyType, std::string> GetType() const;
};

class UE_UByteProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;

	UE_UEnum GetEnum() const;
	std::string GetType() const;

	static UE_UClass StaticClass();
};

class UE_UEnumProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;

	UE_UEnum GetEnum() const;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_UInt16Property : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_UInt32Property : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_UInt64Property : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_Int8Property : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_Int16Property : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_IntProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_Int64Property : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_FloatProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_DoubleProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_BoolProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	uint8 GetFieldMask() const;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_ObjectPropertyBase : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	UE_UClass GetPropertyClass() const;
	static UE_UClass StaticClass();
};

class UE_ObjectProperty : public UE_ObjectPropertyBase
{
public:
	using UE_ObjectPropertyBase::UE_ObjectPropertyBase;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_ClassProperty : public UE_ObjectProperty
{
public:
	using UE_ObjectProperty::UE_ObjectProperty;
	UE_UClass GetMetaClass() const;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_InterfaceProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	UE_UClass GetInterfaceClass() const;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_WeakObjectProperty : public UE_ObjectPropertyBase
{
public:
	using UE_ObjectPropertyBase::UE_ObjectPropertyBase;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_LazyObjectProperty : public UE_ObjectPropertyBase
{
public:
	using UE_ObjectPropertyBase::UE_ObjectPropertyBase;
	std::string GetType() const;

	static UE_UClass StaticClass();
};

class UE_AssetObjectProperty : public UE_ObjectPropertyBase
{
public:
	using UE_ObjectPropertyBase::UE_ObjectPropertyBase;
	std::string GetType() const;

	static UE_UClass StaticClass();
};

class UE_AssetClassProperty : public UE_AssetObjectProperty
{
public:
	using UE_AssetObjectProperty::UE_AssetObjectProperty;

	UE_UClass GetMetaClass() const;
	std::string GetType() const;

	static UE_UClass StaticClass();
};

class UE_NameProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;

	static UE_UClass StaticClass();
};

class UE_StructProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;

	UE_UStruct GetStruct() const;
	std::string GetType() const;

	static UE_UClass StaticClass();
};

class UE_StrProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;

	static UE_UClass StaticClass();
};

class UE_TextProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;

	static UE_UClass StaticClass();
};

class UE_ArrayProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;

	UE_UProperty GetInner() const;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_MapProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;

	UE_UProperty GetKeyProp() const;
	UE_UProperty GetValueProp() const;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_DelegateProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	UE_UFunction GetSignatureFunction() const;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

class UE_MulticastDelegateProperty : public UE_UProperty
{
public:
	using UE_UProperty::UE_UProperty;
	std::string GetType() const;
	static UE_UClass StaticClass();
};

template<typename T>
bool UE_UObject::IsA() const
{
	auto cmp = T::StaticClass();
	if (!cmp) { return false; }

	for (auto super = GetClass(); super; super = super.GetSuper().Cast<UE_UClass>())
	{
		if (super.object == cmp.object)
		{
			return true;
		}
	}

	return false;
}

class UE_UPackage
{
private:
	struct Member
	{
		std::string Name;
		int32 Offset = 0;
		int32 Size = 0;
	};
	struct Function
	{
		std::string FullName;
		std::string CppName;
		std::string Params;
		std::string Flags;
		uint64 Func;
	};
	struct Struct
	{
		std::string FullName;
		std::string CppName;
		int32 Inherited = 0;
		int32 Size = 0;
		std::vector<Member> Members;
		std::vector<Function> Functions;
	};
	struct Enum
	{
		std::string FullName;
		std::string CppName;
		std::vector<std::string> Members;
	};
private:
	std::pair<uint8* const, std::vector<UE_UObject>>* Package;
	std::vector<Struct> Classes;
	std::vector<Struct> Structures;
	std::vector<Enum> Enums;
private:
	static void GenerateBitPadding(std::vector<Member>* members, int32 offset, uint8 bitOffset, uint8 size);
	static void GeneratePadding(std::vector<Member>* members, int32* minOffset, uint8* bitOffset, int32 maxOffset);
	static void GenerateStruct(UE_UStruct object, std::vector<Struct>& arr);
	static void GenerateEnum(UE_UEnum object, std::vector<Enum>& arr);
	static void SaveStruct(std::vector<Struct>& arr, FILE* file);
	static void SaveEnum(std::vector<Enum>& arr, FILE* file);
public:
	UE_UPackage(std::pair<uint8* const, std::vector<UE_UObject>>& package) : Package(&package) {};
	void Process();
	bool Save(const fs::path& dir);
	UE_UObject GetObject() const;
};
