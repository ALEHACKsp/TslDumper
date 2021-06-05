#include "wrappers.h"
#include "memory.h"
#include "engine.h"
#include <fmt/core.h>

std::string UE_FNameEntry::String() const
{
	char buf[1024]{};
	String(buf, 1024);
	return buf;
}

void UE_FNameEntry::String(char* buf, uint16 len) const
{
	Read(object + defs.FNameEntry.HeaderSize, buf, len);
}

std::string UE_FName::GetName() const
{
	uint32 index = Read<uint32>(object);
	if (enc && DecryptNameIndex) { index = DecryptNameIndex(index); }
	auto entry = UE_FNameEntry(GlobalNames.GetEntry(index));
	auto name = entry.String();

	uint32 number = Read<uint32>(object + 4);
	if (enc && DecryptNameNumber) { number = DecryptNameNumber(number); }
	if (number > 0)
	{
		name += '_' + std::to_string(number);
	}

	auto pos = name.rfind('/');
	if (pos != std::string::npos)
	{
		name = name.substr(pos + 1);
	}
	return name;
}

uint32 UE_UObject::GetIndex() const
{
	return DecryptInternalIndex(Read<uint32>(object + defs.UObject.Index));
};

UE_UClass UE_UObject::GetClass() const
{
	return DecryptClass(Read<uint64>(object + defs.UObject.Class));
}

UE_UObject UE_UObject::GetOuter() const
{
	return DecryptOuter(Read<uint64>(object + defs.UObject.Outer));
}

UE_UObject UE_UObject::GetPackageObject() const
{
	UE_UObject package(nullptr);
	for (auto outer = GetOuter(); outer; outer = outer.GetOuter())
	{
		package = outer;
	}
	return package;
}

std::string UE_UObject::GetName() const
{
	auto fname = UE_FName(object + defs.UObject.Name, true);
	return fname.GetName();
}

std::string UE_UObject::GetFullName() const
{
	std::string temp;
	for (auto outer = GetOuter(); outer; outer = outer.GetOuter())
	{
		temp = outer.GetName() + "." + temp;
	}
	UE_UClass objectClass = GetClass();
	std::string name = objectClass.GetName() + " " + temp + GetName();
	return name;
}

std::string UE_UObject::GetCppName() const
{
	std::string name;
	if (IsA<UE_UClass>())
	{
		for (auto c = Cast<UE_UStruct>(); c; c = c.GetSuper())
		{
			if (c == UE_AActor::StaticClass())
			{
				name = "A";
				break;
			}
			else if (c == UE_UObject::StaticClass())
			{
				name = "U";
				break;
			}
		}
	}
	else
	{
		name = "F";
	}

	name += GetName();
	return name;
}

UE_UClass UE_UObject::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.Object");
	return obj;
}

UE_UClass UE_AActor::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class Engine.Actor");
	return obj;
}

UE_UField UE_UField::GetNext() const
{
	return Read<UE_UField>(object + defs.UField.Next);
}

UE_UClass UE_UField::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.Field");
	return obj;
}

UE_UStruct UE_UStruct::GetSuper() const
{
	return Read<UE_UStruct>(object + defs.UStruct.SuperStruct);
}
UE_UField UE_UStruct::GetChildren() const
{
	return Read<UE_UField>(object + defs.UStruct.Children);
}
int32 UE_UStruct::GetSize() const
{
	return Read<int32>(object + defs.UStruct.PropertiesSize);
};
UE_UClass UE_UStruct::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.Struct");
	return obj;
};

uint64 UE_UFunction::GetFunc() const
{
	return Read<uint64>(object + defs.UFunction.Func);
}

std::string UE_UFunction::GetFunctionFlags() const
{
	auto flags = Read<uint32>(object + defs.UFunction.FunctionFlags);
	std::string result;
	if (flags && FUNC_None) { result = "None"; }
	else
	{
		if (flags & FUNC_Final) { result += "Final|"; }
		if (flags & FUNC_RequiredAPI) { result += "RequiredAPI|"; }
		if (flags & FUNC_BlueprintAuthorityOnly) { result += "BlueprintAuthorityOnly|"; }
		if (flags & FUNC_BlueprintCosmetic) { result += "BlueprintCosmetic|"; }
		if (flags & FUNC_Net) { result += "Net|"; }
		if (flags & FUNC_NetReliable) { result += "NetReliable"; }
		if (flags & FUNC_NetRequest) { result += "NetRequest|"; }
		if (flags & FUNC_Exec) { result += "Exec|"; }
		if (flags & FUNC_Native) { result += "Native|"; }
		if (flags & FUNC_Event) { result += "Event|"; }
		if (flags & FUNC_NetResponse) { result += "NetResponse|"; }
		if (flags & FUNC_Static) { result += "Static|"; }
		if (flags & FUNC_NetMulticast) { result += "NetMulticast|"; }
		if (flags & FUNC_UbergraphFunction) { result += "UbergraphFunction|"; }
		if (flags & FUNC_MulticastDelegate) { result += "MulticastDelegate|"; }
		if (flags & FUNC_Public) { result += "Public|"; }
		if (flags & FUNC_Private) { result += "Private|"; }
		if (flags & FUNC_Protected) { result += "Protected|"; }
		if (flags & FUNC_Delegate) { result += "Delegate|"; }
		if (flags & FUNC_NetServer) { result += "NetServer|"; }
		if (flags & FUNC_HasOutParms) { result += "HasOutParms|"; }
		if (flags & FUNC_HasDefaults) { result += "HasDefaults|"; }
		if (flags & FUNC_NetClient) { result += "NetClient|"; }
		if (flags & FUNC_DLLImport) { result += "DLLImport|"; }
		if (flags & FUNC_BlueprintCallable) { result += "BlueprintCallable|"; }
		if (flags & FUNC_BlueprintEvent) { result += "BlueprintEvent|"; }
		if (flags & FUNC_BlueprintPure) { result += "BlueprintPure|"; }
		if (flags & FUNC_EditorOnly) { result += "EditorOnly|"; }
		if (flags & FUNC_Const) { result += "Const|"; }
		if (flags & FUNC_NetValidate) { result += "NetValidate|"; }
		if (result.size()) { result.erase(result.size() - 1); };
	}
	return result;
}

UE_UClass UE_UFunction::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.Struct");
	return obj;
}

UE_UClass UE_UScriptStruct::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.ScriptStruct");
	return obj;
}

UE_UClass UE_UClass::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.Class");
	return obj;
};

TArray UE_UEnum::GetNames() const
{
	return Read<TArray>(object + defs.UEnum.Names);
}

UE_UClass UE_UEnum::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.Enum");
	return obj;
}

int32 UE_UProperty::GetOffset() const
{
	return Read<int32>(object + defs.UProperty.Offset);
}

uint64 UE_UProperty::GetPropertyFlags() const
{
	return Read<uint64>(object + defs.UProperty.PropertyFlags);
}

int32 UE_UProperty::GetArrayDim() const
{
	return Read<int32>(object + defs.UProperty.ArrayDim);
}

int32 UE_UProperty::GetSize() const
{
	return Read<int32>(object + defs.UProperty.ElementSize);
}

std::pair<PropertyType, std::string> UE_UProperty::GetType() const
{
	if (IsA<UE_ArrayProperty>())
	{
		return { PropertyType::ArrayProperty, Cast<UE_ArrayProperty>().GetType() };
	}
	if (IsA<UE_BoolProperty>())
	{
		return { PropertyType::BoolProperty, Cast<UE_BoolProperty>().GetType() };
	}
	if (IsA<UE_UEnumProperty>())
	{
		return { PropertyType::EnumProperty, Cast<UE_UEnumProperty>().GetType() };
	}
	if (IsA<UE_UByteProperty>())
	{
		return { PropertyType::ByteProperty, Cast<UE_UByteProperty>().GetType() };
	}
	if (IsA<UE_ObjectProperty>())
	{
		return { PropertyType::ObjectProperty, Cast<UE_ObjectProperty>().GetType() };
	}
	if (IsA<UE_ClassProperty>())
	{
		return { PropertyType::ClassProperty, Cast<UE_ClassProperty>().GetType() };
	}
	if (IsA<UE_DoubleProperty>())
	{
		return { PropertyType::DoubleProperty, Cast<UE_DoubleProperty>().GetType() };
	}
	if (IsA<UE_FloatProperty>())
	{
		return { PropertyType::FloatProperty, Cast<UE_FloatProperty>().GetType() };
	}
	if (IsA<UE_IntProperty>())
	{
		return { PropertyType::IntProperty, Cast<UE_IntProperty>().GetType() };
	}
	if (IsA<UE_Int16Property>())
	{
		return { PropertyType::Int16Property, Cast<UE_Int16Property>().GetType() };
	}
	if (IsA<UE_Int64Property>())
	{
		return { PropertyType::Int64Property, Cast<UE_Int64Property>().GetType() };
	}
	if (IsA<UE_Int8Property>())
	{
		return { PropertyType::Int8Property, Cast<UE_Int8Property>().GetType() };
	}
	if (IsA<UE_InterfaceProperty>())
	{
		return { PropertyType::InterfaceProperty, Cast<UE_InterfaceProperty>().GetType() };
	}
	if (IsA<UE_MapProperty>())
	{
		return { PropertyType::MapProperty, Cast<UE_MapProperty>().GetType() };
	}
	if (IsA<UE_MulticastDelegateProperty>())
	{
		return { PropertyType::MulticastDelegateProperty, Cast<UE_MulticastDelegateProperty>().GetType() };
	}
	if (IsA<UE_NameProperty>())
	{
		return { PropertyType::NameProperty, Cast<UE_NameProperty>().GetType() };
	}
	if (IsA<UE_StrProperty>())
	{
		return { PropertyType::StrProperty, Cast<UE_StrProperty>().GetType() };
	}
	if (IsA<UE_StructProperty>())
	{
		return { PropertyType::StructProperty, Cast<UE_StructProperty>().GetType() };
	}
	if (IsA<UE_UInt16Property>())
	{
		return { PropertyType::UInt16Property, Cast<UE_UInt16Property>().GetType() };
	}
	if (IsA<UE_UInt32Property>())
	{
		return { PropertyType::UInt32Property, Cast<UE_UInt32Property>().GetType() };
	}
	if (IsA<UE_UInt64Property>())
	{
		return { PropertyType::UInt64Property, Cast<UE_UInt64Property>().GetType() };
	}
	if (IsA<UE_WeakObjectProperty>())
	{
		return { PropertyType::WeakObjectProperty, Cast<UE_WeakObjectProperty>().GetType() };
	}
	if (IsA<UE_TextProperty>())
	{
		return { PropertyType::TextProperty, Cast<UE_TextProperty>().GetType() };
	}

	return { PropertyType::Unknown, GetClass().GetName() };
}

UE_UEnum UE_UByteProperty::GetEnum() const
{
	return Read<UE_UEnum>(object + defs.UProperty.Size);
}

std::string UE_UByteProperty::GetType() const
{
	auto e = GetEnum();
	if (e) { return "enum class " + GetEnum().GetName(); }
	return "bool";
}

UE_UClass UE_UByteProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.ByteProperty");
	return obj;
}

UE_UEnum UE_UEnumProperty::GetEnum() const
{
	return Read<UE_UEnum>(object + defs.UProperty.Size + 8);
}

std::string UE_UEnumProperty::GetType() const
{
	return "enum class " + GetEnum().GetName();
}

UE_UClass UE_UEnumProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.EnumProperty");
	return obj;
}

std::string UE_UInt16Property::GetType() const
{
	return "uint16";
}

UE_UClass UE_UInt16Property::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.UInt16Property");
	return obj;
}

std::string UE_UInt32Property::GetType() const
{
	return "uint32";
}

UE_UClass UE_UInt32Property::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.UInt32Property");
	return obj;
}

std::string UE_UInt64Property::GetType() const
{
	return "uint64";
}

UE_UClass UE_UInt64Property::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.UInt64Property");
	return obj;
}

std::string UE_Int8Property::GetType() const
{
	return "int8";
}

UE_UClass UE_Int8Property::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.Int8Property");
	return obj;
}

std::string UE_Int16Property::GetType() const
{
	return "int16";
}

UE_UClass UE_Int16Property::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.Int16Property");
	return obj;
}

std::string UE_IntProperty::GetType() const
{
	return "int32";
}

UE_UClass UE_IntProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.IntProperty");
	return obj;
}

std::string UE_Int64Property::GetType() const
{
	return "int64";
}

UE_UClass UE_Int64Property::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.Int64Property");
	return obj;
}

std::string UE_FloatProperty::GetType() const
{
	return "float";
}

UE_UClass UE_FloatProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.FloatProperty");
	return obj;
}

std::string UE_DoubleProperty::GetType() const
{
	return "double";
}

UE_UClass UE_DoubleProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.DoubleProperty");
	return obj;
}

uint8 UE_BoolProperty::GetFieldMask() const
{
	return Read<uint8>(object + defs.UProperty.Size + 3);
}

std::string UE_BoolProperty::GetType() const
{
	if (GetFieldMask() == 0xFF)
	{
		return "bool";
	}
	return "char";
}

UE_UClass UE_BoolProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.BoolProperty");
	return obj;
}

UE_UClass UE_ObjectPropertyBase::GetPropertyClass() const
{
	return Read<UE_UClass>(object + defs.UProperty.Size);
}

UE_UClass UE_ObjectPropertyBase::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.ObjectPropertyBase");
	return obj;
}

std::string UE_ObjectProperty::GetType() const
{
	return "struct " + GetPropertyClass().GetCppName() + "*";
}

UE_UClass UE_ObjectProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.ObjectPropertyBase");
	return obj;
}

UE_UClass UE_ClassProperty::GetMetaClass() const
{
	return Read<UE_UClass>(object + defs.UProperty.Size + 8);
}

std::string UE_ClassProperty::GetType() const
{
	return "struct " + GetMetaClass().GetCppName() + "*";
}

UE_UClass UE_ClassProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.ClassProperty");
	return obj;
}

UE_UClass UE_InterfaceProperty::GetInterfaceClass() const
{
	return Read<UE_UClass>(object + defs.UProperty.Size);
}

std::string UE_InterfaceProperty::GetType() const
{
	return "TScriptInterface<struct " + GetInterfaceClass().GetCppName() + ">";
}

UE_UClass UE_InterfaceProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.InterfaceProperty");
	return obj;
}

std::string UE_WeakObjectProperty::GetType() const
{
	return "TWeakObjectPtr<struct " + GetPropertyClass().GetCppName() + ">";
}

UE_UClass UE_WeakObjectProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.WeakObjectProperty");
	return obj;
}

std::string UE_LazyObjectProperty::GetType() const
{
	return "TLazyObjectPtr<struct " + GetPropertyClass().GetCppName() + ">";
}

UE_UClass UE_LazyObjectProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.LazyObjectProperty");
	return obj;
}

std::string UE_AssetObjectProperty::GetType() const
{
	return "TAssetPtr<struct " + GetPropertyClass().GetCppName() + ">";;
}

UE_UClass UE_AssetObjectProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.AssetObjectProperty");
	return obj;
}

UE_UClass UE_AssetClassProperty::GetMetaClass() const
{
	return Read<UE_UClass>(object + defs.UProperty.Size);
}

std::string UE_AssetClassProperty::GetType() const
{
	return "struct " + GetMetaClass().GetCppName() + "*";
}

UE_UClass UE_AssetClassProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.AssetClassProperty");
	return obj;
}

std::string UE_NameProperty::GetType() const
{
	return  "struct FName";
}

UE_UClass UE_NameProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.NameProperty");
	return obj;
}

UE_UStruct UE_StructProperty::GetStruct() const
{
	return Read<UE_UStruct>(object + defs.UProperty.Size);
}

std::string UE_StructProperty::GetType() const
{
	return "struct " + GetStruct().GetCppName();
}

UE_UClass UE_StructProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.StructProperty");
	return obj;
}

std::string UE_StrProperty::GetType() const
{
	return "struct FString";
}

UE_UClass UE_StrProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.StrProperty");
	return obj;
}

std::string UE_TextProperty::GetType() const
{
	return "struct FText";
}

UE_UClass UE_TextProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.TextProperty");
	return obj;
}

UE_UProperty UE_ArrayProperty::GetInner() const
{
	return Read<UE_UProperty>(object + defs.UProperty.Size);
}

std::string UE_ArrayProperty::GetType() const
{
	return "struct TArray<" + GetInner().GetType().second + ">";
}

UE_UClass UE_ArrayProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.ArrayProperty");
	return obj;
}

UE_UProperty UE_MapProperty::GetKeyProp() const
{
	return Read<UE_UProperty>(object + defs.UProperty.Size);
}

UE_UProperty UE_MapProperty::GetValueProp() const
{
	return Read<UE_UProperty>(object + defs.UProperty.Size + 8u);
}

std::string UE_MapProperty::GetType() const
{
	return fmt::format("struct TMap<{}, {}>", GetKeyProp().GetType().second, GetValueProp().GetType().second);
}

UE_UClass UE_MapProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.MapProperty");
	return obj;
}

UE_UClass UE_DelegateProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.DelegateProperty");
	return obj;
}

std::string UE_MulticastDelegateProperty::GetType() const
{
	return "struct FMulticastDelegate";
}

UE_UClass UE_MulticastDelegateProperty::StaticClass()
{
	static auto obj = ObjObjects.FindObject("Class CoreUObject.MulticastDelegateProperty");
	return obj;
}

void UE_UPackage::GenerateBitPadding(std::vector<Member>* members, int32 offset, uint8 bitOffset, uint8 size)
{
	Member padding;
	padding.Name = fmt::format("char pad_{:0X}_{} : {}", offset, bitOffset, size);
	padding.Offset = offset;
	padding.Size = 1;
	members->push_back(padding);
}

void UE_UPackage::GeneratePadding(std::vector<Member>* members, int32* offset, uint8* bitOffset, int32 maxOffset)
{
	if (*bitOffset)
	{
		if (*bitOffset < 8) { UE_UPackage::GenerateBitPadding(members, *offset, *bitOffset, 8 - *bitOffset); }
		*bitOffset = 0;
		*offset += 1;
	}
	if (maxOffset > *offset)
	{
		Member padding;
		auto size = maxOffset - *offset;
		padding.Name = fmt::format("char pad_{:0X}[{:#0x}]", *offset, size);
		padding.Offset = *offset;
		padding.Size = size;
		members->push_back(padding);
		*offset = maxOffset;
	}
}

void UE_UPackage::GenerateStruct(UE_UStruct object, std::vector<Struct>& arr)
{
	Struct s;
	s.Size = object.GetSize();
	if (s.Size == 0) { return; }
	s.Inherited = 0;
	s.FullName = object.GetFullName();
	s.CppName = "struct " + object.GetCppName();

	auto super = object.GetSuper();
	if (super)
	{
		s.CppName += " : " + super.GetCppName();
		s.Inherited = super.GetSize();
	}

	int32 offset = s.Inherited;
	uint8 bitOffset = 0;
	for (auto prop = object.GetChildren().Cast<UE_UProperty>(); prop; prop = prop.GetNext().Cast<UE_UProperty>())
	{
		if (prop.IsA<UE_UFunction>())
		{
			auto fn = prop.Cast<UE_UFunction>();
			Function f;
			f.FullName = fn.GetFullName();
			f.Flags = fn.GetFunctionFlags();
			f.Func = fn.GetFunc();

			for (auto param = fn.GetChildren().Cast<UE_UProperty>(); param; param = param.GetNext().Cast<UE_UProperty>())
			{
				auto flags = param.GetPropertyFlags();
				if (flags & 0x400) // if property has 'ReturnParm' flag
				{
					f.CppName = param.GetType().second + " " + fn.GetName();
				}
				else if (flags & 0x80) // if property has 'Parm' flag
				{
					if (param.GetArrayDim() > 1)
					{
						f.Params += fmt::format("{}* {}, ", param.GetType().second, param.GetName());
					}
					else
					{
						f.Params += fmt::format("{} {}, ", param.GetType().second, param.GetName());
					}
				}
			}

			if (f.Params.size())
			{
				f.Params.erase(f.Params.size() - 2);
			}

			if (f.CppName.size() == 0)
			{
				f.CppName = "void " + fn.GetName();
			}

			s.Functions.push_back(f);
		}
		else
		{
			auto arrDim = prop.GetArrayDim();
			Member m;
			m.Size = prop.GetSize() * arrDim;
			if (m.Size == 0) { return; }
			auto type = prop.GetType();
			m.Name = type.second + " " + prop.GetName();
			m.Offset = prop.GetOffset();

			if (m.Offset > offset)
			{
				UE_UPackage::GeneratePadding(&s.Members, &offset, &bitOffset, m.Offset);
			}

			if (type.first == PropertyType::BoolProperty && *(uint32*)type.second.data() != 'loob')
			{
				auto boolProp = prop.Cast<UE_BoolProperty>();
				auto mask = boolProp.GetFieldMask();
				int zeros = 0, ones = 0;
				while (mask & ~1) { mask >>= 1; zeros++; }
				while (mask & 1) { mask >>= 1; ones++; }
				if (zeros > bitOffset)
				{
					UE_UPackage::GenerateBitPadding(&s.Members, offset, bitOffset, zeros - bitOffset);
					bitOffset = zeros;
				}
				m.Name += fmt::format(" : {}", ones);
				bitOffset += ones;
			}
			else
			{
				if (arrDim > 1)
				{
					m.Name += fmt::format("[{:#04x}]", arrDim);
				}

				offset += m.Size;
			}

			s.Members.push_back(m);
		}
	}

	if (s.Size > offset)
	{
		UE_UPackage::GeneratePadding(&s.Members, &offset, &bitOffset, s.Size);
	}

	arr.push_back(s);
}

void UE_UPackage::GenerateEnum(UE_UEnum object, std::vector<Enum>& arr)
{
	Enum e;
	e.FullName = object.GetFullName();
	e.CppName = "enum class " + object.GetName() + " : uint8";
	auto names = object.GetNames();
	for (auto i = 0ull; i < names.Count; i++)
	{
		auto name = UE_FName(names.Data + i * 0x10, false);
		auto str = name.GetName();

		auto pos = str.find_last_of(':');
		if (pos != std::string::npos)
		{
			str = str.substr(pos + 1);
		}

		e.Members.push_back(str);
	}

	if (e.Members.size())
	{
		arr.push_back(e);
	}
}

void UE_UPackage::SaveStruct(std::vector<Struct>& arr, FILE* file)
{
	for (auto& s : arr)
	{
		fmt::print(file, "// {}\n// Size: {:#04x} (Inherited: {:#04x})\n{} {{", s.FullName, s.Size, s.Inherited, s.CppName);
		for (auto& m : s.Members)
		{
			fmt::print(file, "\n\t{}; // {:#04x}({:#04x})", m.Name, m.Offset, m.Size);
		}
		if (s.Functions.size())
		{
			fwrite("\n", 1, 1, file);
			for (auto& f : s.Functions)
			{
				fmt::print(file, "\n\t{}({}); // {} // {} // @ game+{:#08x}", f.CppName, f.Params, f.FullName, f.Flags, f.Func - Base);
			}
		}

		fmt::print(file, "\n}};\n\n");
	}
}

void UE_UPackage::SaveEnum(std::vector<Enum>& arr, FILE* file)
{
	for (auto& e : arr)
	{
		fmt::print(file, "// {}\n{} {{", e.FullName, e.CppName);
		for (auto& m : e.Members)
		{
			fmt::print(file, "\n\t{},", m);
		}
		fmt::print(file, "\n}};\n\n");
	}
}

void UE_UPackage::Process()
{
	auto& objects = Package->second;
	for (auto& object : objects)
	{
		if (object.IsA<UE_UClass>())
		{
			GenerateStruct(object.Cast<UE_UStruct>(), Classes);
		}
		else if (object.IsA<UE_UScriptStruct>())
		{
			GenerateStruct(object.Cast<UE_UStruct>(), Structures);
		}
		else if (object.IsA<UE_UEnum>())
		{
			GenerateEnum(object.Cast<UE_UEnum>(), Enums);
		}
	}
}

bool UE_UPackage::Save(const fs::path& dir)
{
	if (!(Classes.size() || Structures.size() || Enums.size()))
	{
		return false;
	}

	std::string packageName = GetObject().GetName();

	char chars[] = "/\\:*?\"<>|";
	for (auto c : chars)
	{
		auto pos = packageName.find(c);
		if (pos != std::string::npos) { packageName[pos] = '_'; }
	}

	if (Classes.size())
	{
		File file(dir / (packageName + "_classes.h"), "w");
		if (!file) { return false; }
		UE_UPackage::SaveStruct(Classes, file);
	}

	if (Structures.size() || Enums.size())
	{
		File file(dir / (packageName + "_struct.h"), "w");
		if (!file) { return false; }

		if (Enums.size())
		{
			UE_UPackage::SaveEnum(Enums, file);
		}

		if (Structures.size())
		{
			UE_UPackage::SaveStruct(Structures, file);
		}
	}

	return true;
}

UE_UObject UE_UPackage::GetObject() const
{
	return UE_UObject(Package->first);
}