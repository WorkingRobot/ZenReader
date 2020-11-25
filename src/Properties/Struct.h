#pragma once

#include "../Exceptions/BaseException.h"
#include "../Exports/UObject.h"
#include "../Structs/FLinearColor.h"
#include "../Structs/FVector2D.h"
#include "Base.h"

#include <any>

namespace Zen::Properties {
	using namespace Structs;
	using namespace Exceptions;

	class StructProperty : public BaseProperty {
	public:
		std::any Value;

		StructProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData) {
			switch (Helpers::Hash::Crc32(PropData.GetStructType().c_str(), PropData.GetStructType().size()))
			{
#define CASE(Name, Type) case Helpers::Hash::Crc32(Name): InputStream >> Value.emplace<Type>(); break

				//CASE("LevelSequenceObjectReferenceMap", FLevelSequenceObjectReferenceMap);
				//CASE("GameplayTagContainer", FGameplayTagContainer);
				////CASE("GameplayTag", FGameplayTagContainer);
				//CASE("NavAgentSelector", FNavAgentSelectorCustomization);
				//CASE("Quat", FQuat);
				//CASE("Vector4", FVector4);
				CASE("Vector2D", FVector2D);
				//CASE("Margin", FMargin);
				//CASE("Box2D", FBox2D);
				//CASE("Box", FBox);
				//CASE("Vector", FVector);
				//CASE("Rotator", FRotator);
				//CASE("IntPoint", FIntPoint);
				//CASE("Guid", FGuid);
				//CASE("SoftObjectPath", FSoftObjectPath);
				//CASE("SoftClassPath", FSoftObjectPath);
				//CASE("Color", FColor);
				CASE("LinearColor", FLinearColor);
				//CASE("SimpleCurveKey", FSimpleCurveKey);
				//CASE("RichCurveKey", FRichCurveKey);
				//CASE("FrameNumber", FFrameNumber);
				//CASE("SmartName", FSmartName);
				//CASE("PerPlatformFloat", FPerPlatformFloat);
				//CASE("PerPlatformInt", FPerPlatformInt);
				//CASE("DateTime", FDateTime);
				//CASE("Timespan", FDateTime);
				//CASE("MovieSceneTrackIdentifier", FFrameNumber);
				//CASE("MovieSceneSegmentIdentifier", FFrameNumber);
				//CASE("MovieSceneSequenceID", FFrameNumber);
				//CASE("MovieSceneSegment", FMovieSceneSegment);
				//CASE("SectionEvaluationDataTree", FSectionEvaluationDataTree);
				//CASE("MovieSceneFrameRange", FMovieSceneFrameRange);
				//CASE("MovieSceneEvaluationKey", FMovieSceneEvaluationKey);
				//CASE("MovieSceneFloatValue", FRichCurveKey);
				//CASE("MovieSceneFloatChannel", FMovieSceneFloatChannel);
				//CASE("MovieSceneEvaluationTemplate", FMovieSceneEvaluationTemplate);
				////CASE("SkeletalMeshSamplingLODBuiltData", FSkeletalMeshSamplingLODBuiltData);
				//CASE("VectorMaterialInput", FVectorMaterialInput);
				//CASE("ColorMaterialInput", FColorMaterialInput);
				//CASE("ExpressionInput", FMaterialInput);
				//
				//CASE("PrimaryAssetType", FPrimaryAssetType);
				//CASE("PrimaryAssetId", FPrimaryAssetId);

#undef CASE
			default:
				auto Provider = (const Providers::BaseProvider*)InputStream.GetProperty<Streams::PropId::Provider>();
				if (Provider) {
					auto Schema = Provider->GetSchema(PropData.GetStructType());
					if (Schema) {
						this->Value.emplace<std::shared_ptr<Exports::UObject>>(std::make_shared<Exports::UObject>(InputStream, *Schema, Exports::StructFallback));
						break;
					}
					throw SchemaNotFoundException("The schema for struct \"%s\" was not found", PropData.GetStructType().c_str());
				}
				throw SchemaNotFoundException("A schema provider was not given");
			}
		}

		StructProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadArray) : StructProperty(InputStream, PropData) {}

		StructProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadMap) : StructProperty(InputStream, PropData) {}

		StructProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::StructProperty;
		}
	};
}