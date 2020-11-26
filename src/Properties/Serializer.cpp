#include "Serializer.h"

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
//#include "MulticastDelegate.h"
//#include "LazyObject.h"
#include "SoftObject.h"
#include "AssetObject.h"
#include "UInt64.h"
#include "UInt32.h"
#include "UInt16.h"
#include "Int64.h"
#include "Int16.h"
#include "Int8.h"
#include "Map.h"
#include "Set.h"
#include "Enum.h"

#include "../Exceptions/BaseException.h"

namespace Zen::Properties {
	template<EReadType ReadType, typename T, typename... Args>
	__forceinline std::unique_ptr<BaseProperty> Construct(Args&&... PropArgs) {
		switch (ReadType)
		{
		case EReadType::NORMAL:
			return std::make_unique<T>(std::forward<Args>(PropArgs)...);
		case EReadType::ARRAY:
			return std::make_unique<T>(std::forward<Args>(PropArgs)..., ReadArray);
		case EReadType::MAP:
			return std::make_unique<T>(std::forward<Args>(PropArgs)..., ReadMap);
		case EReadType::ZERO:
			return std::make_unique<T>(std::forward<Args>(PropArgs)..., ReadZero);
		}
	}

	template<EReadType ReadType>
	std::unique_ptr<BaseProperty> Serialize(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EPropertyType Type) {
		switch (Type)
		{
#define CASE(Type, ...) case EPropertyType::Type: return Construct<ReadType, Properties::Type>(InputStream, __VA_ARGS__);

			CASE(ByteProperty, PropData);
			CASE(BoolProperty, PropData);
			CASE(IntProperty);
			CASE(FloatProperty);
			CASE(ObjectProperty);
			CASE(NameProperty);
			CASE(DelegateProperty);
			CASE(DoubleProperty);
			CASE(ArrayProperty, PropData);
			CASE(StructProperty, PropData);
			CASE(StrProperty);
			CASE(TextProperty);
			CASE(InterfaceProperty);
			//CASE(MulticastDelegateProperty, PropData);
			//CASE(LazyObjectProperty, PropData);
			CASE(SoftObjectProperty);
			CASE(AssetObjectProperty);
			CASE(UInt64Property);
			CASE(UInt32Property);
			CASE(UInt16Property);
			CASE(Int64Property);
			CASE(Int16Property);
			CASE(Int8Property);
			CASE(MapProperty, PropData);
			CASE(SetProperty, PropData);
			CASE(EnumProperty, PropData);

#undef CASE
		default:
			throw Exceptions::PropertyTypeNotFoundException("The property type is unknown and cannot be deserialized");
		}
	}

	template std::unique_ptr<BaseProperty> Serialize<EReadType::NORMAL>(Streams::BaseStream&, const Providers::BasePropertyData&, EPropertyType);
	template std::unique_ptr<BaseProperty> Serialize<EReadType::ARRAY>(Streams::BaseStream&, const Providers::BasePropertyData&, EPropertyType);
	template std::unique_ptr<BaseProperty> Serialize<EReadType::MAP>(Streams::BaseStream&, const Providers::BasePropertyData&, EPropertyType);
	template std::unique_ptr<BaseProperty> Serialize<EReadType::ZERO>(Streams::BaseStream&, const Providers::BasePropertyData&, EPropertyType);
}