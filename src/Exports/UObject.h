#pragma once

#include "../Enums/EPropertyType.h"
#include "../Structs/FGuid.h"
#include "../Structs/FUnversionedHeader.h"
#include "../Providers/Base.h"
#include "../Properties/Lookup.h"
#include "../Properties/Serializer.h"
#include "../ZSmallMap.h"
#include "UExport.h"

namespace Zen::Exports {
	using namespace Enums;
	using namespace Structs;

	struct EStructFallback {
		explicit EStructFallback() = default;
	};

	inline constexpr EStructFallback StructFallback{};

	class UObject : public UExport {
	public:
		UObject(UObject&& other) : Props(std::move(other.Props)) {}

		UObject(Streams::BaseStream& InputStream, const Providers::BaseSchema& Schema) {
			Create<false>(InputStream, Schema);
		}

		UObject(Streams::BaseStream& InputStream, const Providers::BaseSchema& Schema, EStructFallback) {
			Create<true>(InputStream, Schema);
		}

		template<typename T, typename NameType>
		const T* TryGet(const NameType& Name) const {
			auto Itr = Props.SearchValues(Name.c_str(), Name.size());
			if (Itr == Props.end()) {
				return nullptr;
			}
			if constexpr (!std::is_same<T, Properties::BaseProperty>::value) {
				if (Properties::GetType<T>() != Itr->second->GetType()) {
					return nullptr;
				}
			}
			return Itr->second.get();
		}

		template<typename T, size_t Size>
		const T* TryGet(const char(&Name)[Size]) const {
			auto Itr = Props.SearchValues(Name, Size - 1);
			if (Itr == Props.end()) {
				return nullptr;
			}
			if constexpr (!std::is_same<T, Properties::BaseProperty>::value) {
				if (Properties::GetType<T>() != Itr->second->GetType()) {
					return nullptr;
				}
			}
			return (T*)Itr->second.get();
		}

	private:
		template<bool StructFallback>
		void Create(Streams::BaseStream& InputStream, const Providers::BaseSchema& Schema) {
			FUnversionedHeader Header;
			InputStream >> Header;

			Structs::FUnversionedHeader::FIterator Itr(Header);
			if (Itr) {
				do {
					if (!Itr.ShouldSerialize()) {
						continue;
					}
					auto& Prop = Schema[Itr.GetSchemaItr()];
					Props.emplace_back(Prop.GetName().c_str(), Prop.GetName().size(), Properties::Serialize<Properties::EReadType::NORMAL>(InputStream, Prop.GetData(), Prop.GetType()));
				} while (++Itr);
			}

			if constexpr (!StructFallback) {
				int HasGuid;
				InputStream >> HasGuid;
				if (HasGuid) {
					FGuid Guid;
					InputStream >> Guid;
				}
			}
		}

	protected:
		ZSmallMap<StrlenKey<uint8_t>, std::unique_ptr<Properties::BaseProperty>> Props;
	};
}