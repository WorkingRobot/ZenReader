#include "FText.h"

#include "FTextHistory.h"

namespace Zen::Structs {
	Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FText& Value)
	{
		uint32_t Flags;
		InputStream >> Flags;

		// "Assuming" the reader/archive is persistent
		Flags &= (uint8_t)ETextFlag::ConvertedProperty | (uint8_t)ETextFlag::InitializedFromString;

		Value.Flags = (ETextFlag)Flags;

		// Execute if UE4 version is at least VER_UE4_FTEXT_HISTORY

		// The type is serialized during the serialization of the history, during deserialization we need to deserialize it and create the correct history
		int8_t HistoryType;
		InputStream >> HistoryType;

		// https://github.com/EpicGames/UnrealEngine/blob/bf95c2cbc703123e08ab54e3ceccdd47e48d224a/Engine/Source/Runtime/Core/Private/Internationalization/TextHistory.cpp
		// https://github.com/EpicGames/UnrealEngine/blob/bf95c2cbc703123e08ab54e3ceccdd47e48d224a/Engine/Source/Runtime/Core/Private/Internationalization/TextData.h
		switch ((ETextHistoryType)HistoryType)
		{
#define CASE(Type) case ETextHistoryType::Type: { Value.TextData = std::shared_ptr<FTextData>((FTextData*)new FTextHistoryData<ETextHistoryType::Type>()); InputStream >> *(FTextHistoryData<ETextHistoryType::Type>*)Value.TextData.get(); break; }

			CASE(None);
			CASE(Base);
			CASE(NamedFormat);
			CASE(OrderedFormat);
			CASE(ArgumentFormat);
			CASE(AsNumber);
			CASE(AsPercent);
			CASE(AsCurrency);
			CASE(AsDate);
			CASE(AsTime);
			CASE(AsDateTime);
			CASE(Transform);
			CASE(StringTableEntry);
			CASE(TextGenerator);

#undef CASE
		}

		return InputStream;
	}

	void FText::Dump()
	{
		if (!TextData) {
			printf("Empty Text\n");
			return;
		}

		switch (TextData->Type)
		{
		case ETextHistoryType::None:
		{
			auto Data = (FTextHistoryData<ETextHistoryType::None>*)TextData.get();
			printf("ETextHistoryType::None - %s\n", Data->CultureInvariantString.c_str());
			break;
		}
		case ETextHistoryType::Base:
		{
			auto Data = (FTextHistoryData<ETextHistoryType::Base>*)TextData.get();
			printf("ETextHistoryType::Base - %s::%s - %s\n", Data->Key.c_str(), Data->Namespace.c_str(), Data->SourceString.c_str());
			break;
		}
		default:
			printf("Unknown Type: %d\n", (int)TextData->Type);
			break;
		}
	}
}