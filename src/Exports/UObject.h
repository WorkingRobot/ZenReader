#pragma once

#include "../Enums/EPropertyType.h"
#include "../Structs/FGuid.h"
#include "../Structs/FUnversionedHeader.h"
#include "../Providers/Base.h"
#include "../Properties/Lookup.h"
#include "../Properties/Serializer.h"
#include "UExport.h"

namespace Zen::Exports {
	using namespace Enums;
	using namespace Structs;

	struct EStructFallback {
		explicit EStructFallback() = default;
	};

	inline constexpr EStructFallback StructFallback{};

	class UObject : public UExport {
		const Providers::Schema& Schema;
		std::vector<std::pair<std::reference_wrapper<const Providers::Property>, std::unique_ptr<Properties::BaseProperty>>> Properties;

	public:
		UObject(UObject&& other) : Schema(other.Schema), Properties(std::move(other.Properties)) {}

		UObject(Streams::BaseStream& InputStream, const Providers::Schema& Schema) : Schema(Schema) {
			Create<false>(InputStream, Schema);
		}

		UObject(Streams::BaseStream& InputStream, const Providers::Schema& Schema, EStructFallback) : Schema(Schema) {
			Create<true>(InputStream, Schema);
		}

		const Providers::Schema& GetSchema() const {
			return Schema;
		}

		const decltype(UObject::Properties)& GetProperties() const {
			return Properties;
		}

		template<typename T>
		const T* TryGet(const Providers::Property& Prop) const {
			return TryGet<T>(Prop.GetSchemaIdx());
		}

		template<typename T, typename NameType>
		const T* TryGet(const NameType& Name) const {
			constexpr bool IsBase = std::is_same<T, Properties::BaseProperty>::value;

			for (auto& Prop : Properties) {
				if (Prop.first.get().GetName().compare(Name.c_str(), Name.size())) {
					if constexpr (!IsBase) {
						if (Properties::GetType<T>() != Prop.second->GetType()) {
							continue;
						}
					}
					return (const T*)Prop.second.get();
				}
			}
			return nullptr;
		}

		template<typename T, size_t Size>
		const T* TryGet(const char(&Name)[Size]) const {
			constexpr bool IsBase = std::is_same<T, Properties::BaseProperty>::value;

			for (auto& Prop : Properties) {
				if (Prop.first.get().GetName().compare(Name, Size - 1)) {
					if constexpr (!IsBase) {
						if (Properties::GetType<T>() != Prop.second->GetType()) {
							continue;
						}
					}
					return (const T*)Prop.second.get();
				}
			}
			return nullptr;
		}

		template<typename T>
		const T* TryGet(uint16_t SchemaIdx) const {
			constexpr bool IsBase = std::is_same<T, Properties::BaseProperty>::value;

			for (auto& Prop : Properties) {
				if (Prop.first.get().GetSchemaIdx() == SchemaIdx) {
					if constexpr (!IsBase) {
						if (Properties::GetType<T>() != Prop.second->GetType()) {
							break;
						}
					}
					return (const T*)Prop.second.get();
				}
			}
			return nullptr;
		}
	
	private:
		template<bool StructFallback>
		void Create(Streams::BaseStream& InputStream, const Providers::Schema& Schema) {
			FUnversionedHeader Header;
			InputStream >> Header;

			Structs::FUnversionedHeader::FIterator Itr(Header);
			if (Itr) {
				do {
					if (!Itr.ShouldSerialize()) {
						continue;
					}
					auto& Prop = Schema.GetProp(Itr.GetSchemaItr());
					Properties.emplace_back(Prop, Properties::Serialize<Properties::EReadType::NORMAL>(InputStream, Prop.GetData()));
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
	};
}