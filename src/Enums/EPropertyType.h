#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum class EPropertyType : uint8_t {
		ByteProperty,
		BoolProperty,
		IntProperty,
		FloatProperty,
		ObjectProperty,
		NameProperty,
		DelegateProperty,
		DoubleProperty,
		ArrayProperty,
		StructProperty,
		StrProperty,
		TextProperty,
		InterfaceProperty,
		MulticastDelegateProperty,
		WeakObjectProperty, //
		LazyObjectProperty, // When deserialized, these 3 properties will be SoftObjects
		AssetObjectProperty, //
		SoftObjectProperty,
		UInt64Property,
		UInt32Property,
		UInt16Property,
		Int64Property,
		Int16Property,
		Int8Property,
		MapProperty,
		SetProperty,
		EnumProperty,
		FieldPathProperty,

		Unknown = 0xFF
	};
}