#pragma once

#include "../Enums/EPropertyType.h"
#include "Byte.h"
#include "Bool.h"
#include "Int.h"
#include "Float.h"
#include "Object.h"
#include "Name.h"
#include "Delegate.h"
#include "Double.h"
#include "Array.h"
#include "Struct.h"
#include "Str.h"
#include "Text.h"
#include "Interface.h"
#include "MulticastDelegate.h"
#include "WeakObject.h"
#include "LazyObject.h"
#include "AssetObject.h"
#include "SoftObject.h"
#include "UInt64.h"
#include "UInt32.h"
#include "UInt16.h"
#include "Int64.h"
#include "Int16.h"
#include "Int8.h"
#include "Map.h"
#include "Set.h"
#include "Enum.h"
#include "FieldPath.h"

namespace Zen::Properties {
	using namespace Enums;

	template<typename T>
	static constexpr EPropertyType GetType() { return EPropertyType::Unknown; };

#define CASE(Type) template<> static constexpr EPropertyType GetType<Type>() { return EPropertyType::Type; }

	CASE(ByteProperty);
	CASE(BoolProperty);
	CASE(IntProperty);
	CASE(FloatProperty);
	CASE(ObjectProperty);
	CASE(NameProperty);
	CASE(DelegateProperty);
	CASE(DoubleProperty);
	CASE(ArrayProperty);
	CASE(StructProperty);
	CASE(StrProperty);
	CASE(TextProperty);
	CASE(InterfaceProperty);
	CASE(MulticastDelegateProperty);
	// CASE(WeakObjectProperty);
	// CASE(LazyObjectProperty); (Alias of SoftObjectProperty)
	// CASE(AssetObjectProperty);
	CASE(SoftObjectProperty);
	CASE(UInt64Property);
	CASE(UInt32Property);
	CASE(UInt16Property);
	CASE(Int64Property);
	CASE(Int16Property);
	CASE(Int8Property);
	CASE(MapProperty);
	CASE(SetProperty);
	CASE(EnumProperty);
	CASE(FieldPathProperty);

#undef CASE

	static constexpr const char* GetString(EPropertyType Type) {
		switch (Type)
		{
#define CASE(Type) case EPropertyType::Type: return #Type;
		
			CASE(ByteProperty);
			CASE(BoolProperty);
			CASE(IntProperty);
			CASE(FloatProperty);
			CASE(ObjectProperty);
			CASE(NameProperty);
			CASE(DelegateProperty);
			CASE(DoubleProperty);
			CASE(ArrayProperty);
			CASE(StructProperty);
			CASE(StrProperty);
			CASE(TextProperty);
			CASE(InterfaceProperty);
			CASE(MulticastDelegateProperty);
			CASE(WeakObjectProperty);
			CASE(LazyObjectProperty);
			CASE(AssetObjectProperty);
			CASE(SoftObjectProperty);
			CASE(UInt64Property);
			CASE(UInt32Property);
			CASE(UInt16Property);
			CASE(Int64Property);
			CASE(Int16Property);
			CASE(Int8Property);
			CASE(MapProperty);
			CASE(SetProperty);
			CASE(EnumProperty);
			CASE(FieldPathProperty);

#undef CASE
		default:
			return "Unknown";
		}
	}
}