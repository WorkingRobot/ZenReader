#pragma once

#include "../Enums/EPropertyType.h"
#include "../Structs/FGuid.h"
#include "../Structs/FUnversionedHeader.h"
#include "../Providers/Base.h"
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
	public:
		std::vector<std::unique_ptr<Properties::BaseProperty>> Props;

		UObject(UObject&& other) : Props(std::move(other.Props)) {}

		UObject(Streams::BaseStream& InputStream, const Providers::BaseSchema& Schema) {
			Create<false>(InputStream, Schema);
		}

		UObject(Streams::BaseStream& InputStream, const Providers::BaseSchema& Schema, EStructFallback) {
			Create<true>(InputStream, Schema);
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
					Props.emplace_back(Properties::Serialize<Properties::EReadType::NORMAL>(InputStream, Prop.GetData(), Prop.GetType()));
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