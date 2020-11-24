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
		//MulticastDelegateProperty,
		//LazyObjectProperty,
		SoftObjectProperty,
		AssetObjectProperty, // When deserialized, all AssetObjects will be SoftObjects
		UInt64Property,
		UInt32Property,
		UInt16Property,
		Int64Property,
		Int16Property,
		Int8Property,
		MapProperty,
		SetProperty,
		EnumProperty,

		Unknown = 0xFF
	};
}