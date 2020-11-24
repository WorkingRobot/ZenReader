#pragma once

#include "../Streams/BaseStream.h"
#include "../Helpers/Align.h"

namespace Zen::Structs {
	class FUnversionedHeader {
	public:
		// Not defined for public use, we'll just keep it here
		// https://github.com/EpicGames/UnrealEngine/blob/283e412aa843210f2d6e9ed0236861cf749b3429/Engine/Source/Runtime/CoreUObject/Private/Serialization/UnversionedPropertySerialization.cpp#L471
		struct FFragment
		{
		private:
			static constexpr uint32_t SkipMax = 127;
			static constexpr uint32_t ValueMax = 127;

			static constexpr uint32_t SkipNumMask = 0x007fu;
			static constexpr uint32_t HasZeroMask = 0x0080u;
			static constexpr uint32_t ValueNumShift = 9u;
			static constexpr uint32_t IsLastMask = 0x0100u;

		public:
			uint8_t SkipNum; // Number of properties to skip before values
			bool bHasAnyZeroes;
			uint8_t ValueNum;  // Number of subsequent property values stored
			bool bIsLast; // Is this the last fragment of the header?

			FFragment(uint16_t Int) {
				SkipNum = static_cast<uint8_t>(Int & SkipNumMask);
				bHasAnyZeroes = (Int & HasZeroMask) != 0;
				ValueNum = static_cast<uint8_t>(Int >> ValueNumShift);
				bIsLast = (Int & IsLastMask) != 0;
			}
		};

		struct FIterator {
		private:
			std::vector<FFragment>::const_iterator FragmentIterator;
			std::vector<bool>::const_iterator ZeroMaskIterator;
			uint32_t RemainingFragmentValues;
			bool bIsDone;
			uint32_t SchemaIterator;
			bool NonZero;

		public:
			FIterator(const FUnversionedHeader& Header) :
				FragmentIterator(Header.Fragments.cbegin()),
				ZeroMaskIterator(Header.ZeroMask.cbegin()),
				RemainingFragmentValues(0),
				bIsDone(false),
				SchemaIterator(0),
				NonZero(true)
			{
				Skip();
			}

			FIterator& operator++() {
				// Assert !bIsDone
				SchemaIterator++;
				RemainingFragmentValues--;
				if (FragmentIterator->bHasAnyZeroes) {
					ZeroMaskIterator++;
				}

				if (!RemainingFragmentValues) {
					if (FragmentIterator->bIsLast) {
						bIsDone = true;
						return *this;
					}

					FragmentIterator++;
					Skip();
				}
				NonZero = !FragmentIterator->bHasAnyZeroes || !*ZeroMaskIterator;
				return *this;
			}

			uint32_t GetSchemaItr() const {
				return SchemaIterator;
			}

			bool IsValid() const {
				return !bIsDone;
			}

			bool IsNonZero() const {
				return NonZero;
			}

			bool ShouldSerialize() const {
				return IsValid() && IsNonZero();
			}

			operator bool() const {
				return IsValid();
			}

		private:
			void Skip() {
				SchemaIterator += FragmentIterator->SkipNum;

				while (!FragmentIterator->ValueNum) {
					if (FragmentIterator->bIsLast) {
						printf("Iterator error\n");
					}
					// Assert !FragmentIterator->bIsLast
					FragmentIterator++;
					SchemaIterator += FragmentIterator->SkipNum;
				}

				RemainingFragmentValues = FragmentIterator->ValueNum;
			}
		};

		std::vector<FFragment> Fragments;
		std::vector<bool> ZeroMask;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FUnversionedHeader& Value) {
			FFragment* Fragment = nullptr;
			uint32_t ZeroMaskNum = 0;
			uint32_t UnmaskedNum = 0;
			do
			{
				uint16_t Packed;
				InputStream >> Packed;

				Fragment = &Value.Fragments.emplace_back(Packed); // For use outside of scope

				(Fragment->bHasAnyZeroes ? ZeroMaskNum : UnmaskedNum) += Fragment->ValueNum;
			} while (!Fragment->bIsLast);

			if (ZeroMaskNum > 0)
			{
				Value.LoadZeroMaskData(InputStream, ZeroMaskNum);
			}

			return InputStream;
		}

	private:
		void LoadZeroMaskData(Streams::BaseStream& InputStream, int32_t NumBits) {
			printf("Loading zero mask data, not been tested\n");
			// MSVC specific lol
			if (NumBits <= 8) {
				ZeroMask.resize(Helpers::Align<8>(NumBits));
				InputStream >> *(uint8_t*)ZeroMask._Myvec.data();
			}
			else if (NumBits <= 16) {
				ZeroMask.resize(Helpers::Align<16>(NumBits));
				InputStream >> *(uint16_t*)ZeroMask._Myvec.data();
			}
			else {
				ZeroMask.resize(Helpers::Align<32>(NumBits));
				auto Data = (uint32_t*)ZeroMask._Myvec.data();
				do {
					InputStream >> *(Data++);
					NumBits -= 32;
				} while (NumBits > 0);
			}
			ZeroMask.resize(NumBits);
			ZeroMask.shrink_to_fit();
		}
	};
}