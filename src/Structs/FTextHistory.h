#pragma once

#include "../Enums/EDateTimeStyle.h"
#include "../Enums/ETextHistoryType.h"
#include "../Enums/ETransformType.h"
#include "../Exceptions/BaseException.h"
#include "../Streams/BaseStream.h"
#include "FDateTime.h"
#include "FFormatArgumentValue.h"
#include "FFormatNamedArguments.h"
#include "FName.h"
#include "FNumberFormattingOptions.h"
#include "FText.h"
#include "FTextData.h"

#include <optional>

namespace Zen::Structs {
	using namespace Enums;
	using namespace Exceptions;

	template<ETextHistoryType Type>
	struct FTextHistoryData : public FTextData {
		FTextHistoryData() : FTextData(Type) {}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::None> {
		std::string CultureInvariantString;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::None>& Value) {
			int HasCultureInvariantString;
			InputStream >> HasCultureInvariantString;
			if (HasCultureInvariantString) {
				InputStream >> Value.CultureInvariantString;
			}

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::Base> {
		std::string Namespace;
		std::string Key;
		std::string SourceString;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::Base>& Value) {
			InputStream >> Value.Namespace;
			InputStream >> Value.Key;
			InputStream >> Value.SourceString;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::NamedFormat> {
		FText SourceFmt;
		FFormatNamedArguments Arguments;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::NamedFormat>& Value) {
			InputStream >> Value.SourceFmt;
			InputStream >> Value.Arguments;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::OrderedFormat> {
		FText SourceFmt;
		FFormatNamedArguments Arguments;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::OrderedFormat>& Value) {
			InputStream >> Value.SourceFmt;
			InputStream >> Value.Arguments;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::ArgumentFormat> {
		FText SourceFmt;
		std::vector<FFormatArgumentValue> Arguments;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::ArgumentFormat>& Value) {
			InputStream >> Value.SourceFmt;
			InputStream >> Value.Arguments;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::AsNumber> {
		FFormatArgumentValue SourceValue;
		// FModel skips FormatOptions and provides a std::string TimeZone (?)
		std::optional<FNumberFormattingOptions> FormatOptions;
		std::string TargetCulture;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::AsNumber>& Value) {
			InputStream >> Value.SourceValue;

			bool bHasFormatOptions;
			InputStream >> bHasFormatOptions;
			if (bHasFormatOptions) {
				InputStream >> Value.FormatOptions.emplace();
			}

			InputStream >> Value.TargetCulture;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::AsPercent> {
		FFormatArgumentValue SourceValue;
		// FModel skips FormatOptions and provides a std::string TimeZone (?)
		std::optional<FNumberFormattingOptions> FormatOptions;
		std::string TargetCulture;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::AsPercent>& Value) {
			InputStream >> Value.SourceValue;

			bool bHasFormatOptions;
			InputStream >> bHasFormatOptions;
			if (bHasFormatOptions) {
				FNumberFormattingOptions FormatOptions;
				InputStream >> FormatOptions;
				Value.FormatOptions.emplace(std::move(FormatOptions));
			}

			InputStream >> Value.TargetCulture;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::AsCurrency> {
		std::string CurrencyCode;
		FFormatArgumentValue SourceValue;
		// FModel skips FormatOptions and provides a std::string TimeZone (?)
		std::optional<FNumberFormattingOptions> FormatOptions;
		std::string TargetCulture;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::AsCurrency>& Value) {
			InputStream >> Value.CurrencyCode;
			InputStream >> Value.SourceValue;

			bool bHasFormatOptions;
			InputStream >> bHasFormatOptions;
			if (bHasFormatOptions) {
				FNumberFormattingOptions FormatOptions;
				InputStream >> FormatOptions;
				Value.FormatOptions.emplace(std::move(FormatOptions));
			}

			InputStream >> Value.TargetCulture;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::AsDate> {
		FDateTime SourceDateTime;
		EDateTimeStyle DateStyle;
		std::string TimeZone;
		std::string TargetCulture;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::AsDate>& Value) {
			InputStream >> Value.SourceDateTime;

			int8_t DateStyleInt8;
			InputStream >> DateStyleInt8;
			Value.DateStyle = (EDateTimeStyle)DateStyleInt8;

			// UE4Ver >= VER_UE4_FTEXT_FTextHistoryData<ETextHistoryType::DATE_TIMEZONE>
			InputStream >> Value.TimeZone;

			InputStream >> Value.TargetCulture;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::AsTime> {
		FDateTime SourceDateTime;
		EDateTimeStyle TimeStyle;
		std::string TimeZone;
		std::string TargetCulture;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::AsTime>& Value) {
			InputStream >> Value.SourceDateTime;

			int8_t TimeStyleInt8;
			InputStream >> TimeStyleInt8;
			Value.TimeStyle = (EDateTimeStyle)TimeStyleInt8;

			InputStream >> Value.TimeZone;

			InputStream >> Value.TargetCulture;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::AsDateTime> {
		FDateTime SourceDateTime;
		EDateTimeStyle DateStyle;
		EDateTimeStyle TimeStyle;

		std::string TimeZone;

		// UE4 converts the string into an FCulturePtr
		// https://github.com/EpicGames/UnrealEngine/blob/bf95c2cbc703123e08ab54e3ceccdd47e48d224a/Engine/Source/Runtime/Core/Private/Internationalization/TextHistory.cpp#L2188
		std::string TargetCulture;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::AsDateTime>& Value) {
			InputStream >> Value.SourceDateTime;

			int8_t DateStyleInt8;
			InputStream >> DateStyleInt8;
			Value.DateStyle = (EDateTimeStyle)DateStyleInt8;

			int8_t TimeStyleInt8;
			InputStream >> TimeStyleInt8;
			Value.TimeStyle = (EDateTimeStyle)TimeStyleInt8;

			InputStream >> Value.TimeZone;

			InputStream >> Value.TargetCulture;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::Transform> {
		FText SourceText;
		ETransformType TransformType;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::Transform>& Value) {
			InputStream >> Value.SourceText;
			uint8_t TransformType;
			InputStream >> TransformType;
			Value.TransformType = (ETransformType)TransformType;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::StringTableEntry> {
		FName TableId;
		std::string Key;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::StringTableEntry>& Value) {
			InputStream >> Value.TableId;
			InputStream >> Value.Key;

			return InputStream;
		}
	};

	template<>
	struct FTextHistoryData<ETextHistoryType::TextGenerator> {
		FName GeneratorTypeID;
		std::vector<uint8_t> GeneratorContents;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FTextHistoryData<ETextHistoryType::TextGenerator>& Value) {
			InputStream >> Value.GeneratorTypeID;

			auto NameMap = (const ZNameMap*)InputStream.GetProperty<Streams::PropId::NameMap>();
			if (!NameMap) {
				throw NameMapNotFoundException("Name map property isn't set on the stream");
			}
			if (!Value.GeneratorTypeID.IsNone(*NameMap)) {
				InputStream >> Value.GeneratorContents;
			}

			return InputStream;
		}
	};
}